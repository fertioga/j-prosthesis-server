---

description: "Task list for percentual de bateria no anúncio BLE"
---

# Tasks: Percentual de bateria no anúncio

**Input**: Design documents from `/specs/001-battery-level-advertising/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/battery-level-advertising.md, quickstart.md

**Tests**: Não há suíte automatizada nesta feature. A validação é o quickstart manual na placa. Não criar testes de PlatformIO.

**Organization**: Tarefas agrupadas por user story para implementação e teste independentes.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Pode rodar em paralelo (arquivos diferentes, sem dependência de tarefa incompleta)
- **[Story]**: User story da tarefa (US1, US2, US3)
- Cada tarefa inclui caminho de arquivo

## Path Conventions

- Firmware único: `src/app/service/`, `src/app/actions/`, `src/main.cpp`
- Contrato: `specs/001-battery-level-advertising/contracts/battery-level-advertising.md`
- Sem alteração em `platformio.ini`, `data/config.json` ou no repositório do relógio

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Pasta do serviço de carga, sem dependência nova

- [X] T001 Criar o diretório `src/app/service/power/battery/` conforme `specs/001-battery-level-advertising/plan.md`. Não acrescentar biblioteca em `platformio.ini` e não definir `BATTERY_LEVEL_INJECT` em `platformio.ini`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Tipo da leitura e anúncio que ainda cabe o nome e o UUID de comando. Nenhuma user story começa antes disto

**⚠️ CRITICAL**: Nenhuma user story começa antes desta fase

- [X] T002 Criar `src/app/service/power/battery/BatteryLevel.hpp` com os campos de `specs/001-battery-level-advertising/data-model.md`: `valid` (bool, `true` somente com percentual inteiro de 0 a 100), `percent` (inteiro 0–100, obrigatório quando `valid` é `true`, ausente do anúncio quando `valid` é `false`), `sampledAtMs` (instante monotônico da amostra válida, atualizado só quando entra uma amostra válida). Valor bruto abaixo de 0 ou acima de 100 força `valid = false` e não preenche `percent` anunciável. Valor fracionário vira o inteiro mais próximo, com distância máxima de 1 ponto em relação à leitura aceita; 0 e 100 permanecem 0 e 100. A struct não tem identificador de paciente, tensão, flag de carregamento nem histórico
- [X] T003 Criar `src/app/service/power/battery/BatteryLevelService.hpp` com `sample()` retornando `BatteryLevel`. Na imagem de produção `sample()` retorna `valid = false` (carga indisponível). Não ler ADC, não inventar pino em `src/app/configs/HardwareMapConf.hpp`, não devolver o texto `BAT:87`
- [X] T004 [P] Reconstruir o anúncio em `begin()` de `src/app/service/bluetooth/ble/BleService.cpp`: pacote principal com flags, nome completo `J1-Prosthesis` e lista completa de UUID de 16 bits `0x180F`; scan response com a lista completa do UUID de 128 bits `12345678-1234-1234-1234-1234567890ab`. Remover as chamadas `addServiceUUID` que empilham UUID por cima de `setAdvertisementData`. Se `setAdvertisementData` ou `setScanResponseData` retornar false, manter o nome no pacote principal. Ainda sem byte de percentual

**Checkpoint**: Tipo de leitura pronto e o aparelho continua anunciando nome e UUID de comando sem percentual numérico

---

## Phase 3: User Story 1 - Ver a carga sem conectar (Priority: P1) 🎯 MVP

**Goal**: Com uma leitura válida, o anúncio público inclui um inteiro de 0 a 100 que um observador lê sem parear e sem abrir sessão. O nome e o comando de luz continuam funcionando

**Independent Test**: Ligar a prótese com uma leitura válida (injeção local) e, num escaneador que só escuta anúncios, ver o inteiro sem parear e sem conectar. Em seguida achar `J1-Prosthesis` e enviar um comando de luz como em `doc/led_codes.md`

### Implementation for User Story 1

- [X] T005 [P] [US1] Declarar `applyAdvertisedBattery(uint8_t percent)` em `src/app/service/bluetooth/ble/BleService.hpp` e implementar em `src/app/service/bluetooth/ble/BleService.cpp`. Com `percent` de 0 a 100, o pacote principal ganha Service Data AD type `0x16`, UUID `0x180F` e esse byte (42 produz `04 16 0F 18 2A`, conforme `specs/001-battery-level-advertising/contracts/battery-level-advertising.md`). Manter o nome `J1-Prosthesis` e a lista `0x180F`. O pacote principal permanece em no máximo 31 bytes (flags 3 + nome 15 + service data 5 + lista 16 bits 4 = 27). Chamar `refreshAdvertisingData()`. Guardar o último pacote para o `onDisconnect` usar este mesmo builder. Em `ServerCallbacks::onConnect`, chamar `startAdvertising()` de novo com esse pacote; se o controlador recusar, a sessão de LED segue
- [X] T006 [P] [US1] Em `src/app/service/power/battery/BatteryLevelService.hpp`, compilar um caminho de injeção somente se `BATTERY_LEVEL_INJECT` estiver definido. A injeção grava `valid = true` e `percent` no inteiro recebido, desde que esteja em 0–100. Sem o flag, `sample()` continua retornando `valid = false`. Não colocar o flag em `platformio.ini`
- [X] T007 [US1] Criar `src/app/actions/PublishBatteryLevelAction.hpp` e `src/app/actions/PublishBatteryLevelAction.cpp`. `run()` sobe task com `TaskService` e `IAction::entry`, nome diferente de `"LED Task"`, stack 4096, prioridade 1, core 1. A task chama `sample()`; se `valid` for `true`, chama `applyAdvertisedBattery(percent)`. Não escrever na fila de LED, não chamar `notifyBattery` e não logar identificador de paciente no Serial
- [X] T008 [US1] Em `src/main.cpp`, instanciar `BatteryLevelService` e `PublishBatteryLevelAction` e chamar `run()` depois de `ble.begin()`. `main.cpp` permanece só wiring

**Checkpoint**: Com `BATTERY_LEVEL_INJECT` e o valor 42, o escaneador mostra `04 16 0F 18 2A` sem conexão. Sem o flag, não há byte de percentual. O comando de luz continua aceito

---

## Phase 4: User Story 2 - O anúncio acompanha a carga (Priority: P2)

**Goal**: O inteiro anunciado muda quando a carga muda pelo menos 1 ponto, inclusive nos limites 0 e 100, e todos os observadores veem o mesmo pacote

**Independent Test**: Partir de um inteiro conhecido, injetar outro inteiro com diferença de pelo menos 1 ponto e confirmar o valor novo num novo escaneamento em até 60 segundos. Repetir com 0 e com 100. Dois escaneadores simultâneos mostram o mesmo byte

### Implementation for User Story 2

- [X] T009 [P] [US2] Em `src/app/actions/PublishBatteryLevelAction.cpp`, amostrar a cada 10 segundos com `vTaskDelay`. Chamar `applyAdvertisedBattery` só quando o inteiro anunciado muda. Duas amostras com o mesmo inteiro não regravam o rádio (FR-012). O inteiro anunciado é o de `BatteryLevel.percent` (já arredondado: inteiro mais próximo, distância máxima de 1 ponto; 0 e 100 permanecem 0 e 100). Há um único pacote em `BleService`; não existe percentual por conexão
- [X] T010 [P] [US2] Estender a injeção sob `BATTERY_LEVEL_INJECT` em `src/app/service/power/battery/BatteryLevelService.hpp` para substituir o inteiro enquanto a task já está rodando, na ordem 0, depois 100, depois 42. `sample()` passa a devolver o inteiro novo na amostra seguinte. Fora do flag, `sample()` permanece `valid = false`

**Checkpoint**: Trocar a injeção altera o byte do anúncio em até 60 segundos. 0 anunciado é `0x00` e 100 é `0x64`

---

## Phase 5: User Story 3 - Carga desconhecida não vira número falso (Priority: P3)

**Goal**: Sem leitura válida o anúncio não afirma percentual. Um valor velho sai depois de 2 minutos. O comando de luz continua aceito

**Independent Test**: Subir sem leitura válida e confirmar ausência de Service Data de percentual. Com um percentual no ar, invalidar a leitura, ver o byte ainda presente antes de 2 minutos e ausente depois. Enviar um comando de luz nesse estado

### Implementation for User Story 3

- [X] T011 [P] [US3] Declarar `clearAdvertisedBattery()` em `src/app/service/bluetooth/ble/BleService.hpp` e implementar em `src/app/service/bluetooth/ble/BleService.cpp`. O método reconstrói o pacote principal sem o AD `0x16` de `0x180F`, mantém o nome `J1-Prosthesis` e a lista de 16 bits `0x180F`, mantém o UUID de 128 bits no scan response e chama `refreshAdvertisingData()`
- [X] T012 [US3] Em `src/app/actions/PublishBatteryLevelAction.cpp`, tratar `sample()` com `valid = false` (inclui valor bruto abaixo de 0 ou acima de 100). Sem nenhuma amostra válida desde o boot, não chamar `applyAdvertisedBattery` e deixar o anúncio sem byte de percentual. Se já existe amostra válida e `millis() - sampledAtMs < 120000`, manter o último `percent`. Ao atingir 120000 ms sem amostra válida nova, chamar `clearAdvertisedBattery()`. A próxima amostra com `valid = true` volta a chamar `applyAdvertisedBattery` no ciclo de 10 segundos

**Checkpoint**: Boot sem injeção não mostra percentual. Depois de anunciar um inteiro, esse inteiro some aos 2 minutos sem amostra nova, e o LED ainda aceita comando

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Tirar o publicador rejeitado e executar o quickstart

- [X] T013 Remover `src/app/actions/sendBatteryLevel.cpp` para o texto fixo `BAT:87` não poder ser compilado nem ligado a `notifyBattery`. Não gravar a characteristic `0x2A19` nesta feature
- [X] T014 [P] Em `doc/led_codes.md`, acrescentar um ponteiro para `specs/001-battery-level-advertising/contracts/battery-level-advertising.md` no trecho do Battery Service. Não alterar nome, passkey, UUIDs de LED nem o payload de 5 bytes
- [ ] T015 Executar os cenários 1 a 6 de `specs/001-battery-level-advertising/quickstart.md` na placa e registrar o que o escaneador mostrou

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: sem dependências
- **Foundational (Phase 2)**: depende de T001. Bloqueia todas as user stories
- **User Story 1 (Phase 3)**: depende da Phase 2
- **User Story 2 (Phase 4)**: depende da Phase 3, porque altera a task criada em T007
- **User Story 3 (Phase 5)**: depende da Phase 4, porque a expiração roda dentro do ciclo de 10 segundos de T009
- **Polish (Phase 6)**: depende das user stories desejadas. T015 depende de T013 e T014

### User Story Dependencies

- **User Story 1 (P1)**: começa após a Phase 2. Não depende de US2 nem US3
- **User Story 2 (P2)**: começa após US1. Testável sozinha com injeção mutável, sem a expiração de US3
- **User Story 3 (P3)**: começa após US2. Testável com boot sem injeção e com amostra invalidada depois de um inteiro anunciado

### Within Each User Story

- T005 e T006 podem andar juntos; T007 espera os dois; T008 espera T007
- T009 e T010 podem andar juntos
- T011 pode andar junto com o fim de US2; T012 espera T011
- US2 e US3 editam `src/app/actions/PublishBatteryLevelAction.cpp` em sequência, não em paralelo

### Parallel Opportunities

- T002 e T004 podem rodar em paralelo
- T003 espera T002. Se T004 ainda estiver aberto, T003 pode seguir em paralelo com T004
- Depois da Phase 2, T005 e T006 podem rodar em paralelo
- T009 e T010 podem rodar em paralelo
- T013 e T014 podem rodar em paralelo
- US2 não começa em paralelo com US1: os dois escrevem a action

---

## Parallel Example: User Story 1

```bash
Task: "T005 applyAdvertisedBattery em src/app/service/bluetooth/ble/BleService.hpp e BleService.cpp"
Task: "T006 injeção BATTERY_LEVEL_INJECT em src/app/service/power/battery/BatteryLevelService.hpp"
```

## Parallel Example: User Story 2

```bash
Task: "T009 ciclo de 10 segundos em src/app/actions/PublishBatteryLevelAction.cpp"
Task: "T010 injeção substituível em src/app/service/power/battery/BatteryLevelService.hpp"
```

## Parallel Example: User Story 3

```bash
Task: "T011 clearAdvertisedBattery em src/app/service/bluetooth/ble/BleService.hpp e BleService.cpp"
# T012 espera T011 e edita a action; não lançar junto com outra edição desse cpp
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Completar Phase 1 e Phase 2
2. Completar Phase 3 (T005–T008)
3. **PARAR E VALIDAR**: cenário 2 do quickstart (byte de 42 sem conexão) e um comando de luz
4. A imagem sem `BATTERY_LEVEL_INJECT` já cobre o cenário 1 (sem número)

### Incremental Delivery

1. Setup + Foundational → anúncio com nome e UUID, sem percentual
2. User Story 1 → percentual visível sem sessão (MVP)
3. User Story 2 → o byte acompanha 0, 100 e trocas de pelo menos 1 ponto
4. User Story 3 → omissão no boot e expiração em 2 minutos
5. Polish → remover `sendBatteryLevel.cpp` e rodar o quickstart inteiro

### Parallel Team Strategy

1. Uma pessoa faz T002 e T003; outra faz T004
2. Em US1, uma pessoa faz T005 e outra faz T006; T007 e T008 ficam com quem fechar os dois
3. US2 e US3 seguem em série no arquivo da action

---

## Notes

- [P] = arquivos diferentes, sem depender de tarefa incompleta
- O flag `BATTERY_LEVEL_INJECT` existe só no build local de bancada
- `sample()` de produção permanece indisponível até existir sensor de verdade
- Cada história deve poder ser demonstrada sem as histórias de prioridade menor
