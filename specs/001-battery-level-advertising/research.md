# Research: Percentual de bateria no anúncio

## 1. Onde está o percentual

**Decision**: `BatteryLevelService::sample()` é a única fonte. Na imagem de produção desta entrega, `sample()` retorna carga indisponível. O anúncio só ganha o byte quando `sample()` retornar um inteiro de 0 a 100. A validação de bancada injeta esse inteiro com o flag de compilação `BATTERY_LEVEL_INJECT`, que não entra no `platformio.ini`.

**Rationale**: `HardwareMapConf` define LED (GPIO 16) e I2C do acelerômetro (SDA 21, SCL 22). Não há ADC de bateria, divisor resistivo nem fuel gauge. Inventar um GPIO publicaria um número que não é a carga da prótese e quebraria FR-004 e FR-007. O arquivo `sendBatteryLevel.cpp` está comentado e usa o texto fixo `BAT:87`; esse texto não é leitura.

**Alternatives considered**:

- ADC num GPIO livre, com curva em `config.json`. Rejeitado: o pino e a curva não existem no repositório. Calibrar no escuro viola o limite de 1 ponto da spec.
- Publicar `BAT:87` ou outro constante para “ter um número no ar”. Rejeitado por FR-007.
- Characteristic GATT `0x2A19` com notify a cada 5 s, como o esboço comentado. Rejeitado: a spec pede o anúncio, sem sessão. O notify continua fora desta feature. Se um dia os dois canais existirem, os dois usam o mesmo inteiro 0–100 deste contrato.

## 2. Formato no ar

**Decision**: Service Data de 16 bits (AD type `0x16`) com UUID `0x180F` e um `uint8` de 0 a 100. Sem leitura válida, essa estrutura some do pacote. O UUID do serviço pode continuar listado; a lista sozinha não é percentual.

**Rationale**: O firmware já cria o Battery Service `0x180F` e a characteristic `0x2A19`. O Service Data com um byte é o mesmo valor que a characteristic Battery Level carrega no perfil padrão, e cabe em 5 bytes. Escaneadores comuns (nRF Connect, LightBlue) mostram esse campo sem conexão. `NimBLEAdvertisementData::setServiceData` grava esse AD. `NimBLEAdvertising::refreshAdvertisingData` aplica o pacote novo com o anúncio já ligado.

**Alternatives considered**:

- Manufacturer Data com um layout próprio. Rejeitado: o relógio e o celular precisariam de um parser particular, e a constituição pede contrato determinístico alinhado ao que o server já declara.
- String `BAT:87` dentro do anúncio. Rejeitado: não é inteiro 0–100, estoura byte à toa e contradiz FR-007.
- Byte `0xFF` como sentinela de “desconhecido”. Rejeitado: um observador pode tratar `255` como número. A spec manda omitir o percentual.

## 3. Orçamento de 31 bytes

**Decision**: Pacote principal leva flags, nome completo `J1-Prosthesis`, Service Data `0x180F` (quando houver leitura) e a lista completa de UUID de 16 bits `0x180F`. Scan response leva a lista completa do UUID de 128 bits do serviço de comando (`12345678-1234-1234-1234-1234567890ab`). `BleService::begin()` deixa de empilhar `addServiceUUID` por cima desse pacote explícito.

**Rationale**: O anúncio legado tem 31 bytes.

| Campo | Bytes |
|-------|------:|
| Flags | 3 |
| Nome completo `J1-Prosthesis` | 15 |
| Service Data `0x180F` + 1 byte | 5 |
| Lista de UUID 16 bits `0x180F` | 4 |
| **Total** | **27** |

Sobra 4 bytes. O UUID de 128 bits sozinho ocupa 18 bytes e não cabe nesse pacote. O `begin()` atual pede nome e UUID de 128 bits no mesmo anúncio (15 + 18, mais flags), o que já passa de 31. Separar os pacotes é o que mantém o nome e ainda abre espaço para o percentual.

O relógio faz scan ativo e aceita o aparelho pelo nome **ou** pelo UUID de 128 bits (`WatchBleClient`). O nome no pacote principal preserva a descoberta. O UUID no scan response preserva o outro critério, porque o scan do relógio já é ativo.

**Alternatives considered**:

- Percentual só no scan response. Rejeitado: quem escuta só o pacote principal não vê a carga, e o pacote principal é justamente o que cabe com o nome.
- Encurtar o nome. Rejeitado por FR-009. O nome anunciado permanece `J1-Prosthesis`.

## 4. Quando reescrever o anúncio

**Decision**: A action amostra a cada 10 segundos. Chama `refreshAdvertisingData()` somente se o percentual mudou ou se a estrutura passou a existir ou a sumir. A última leitura válida continua no ar por até 2 minutos sem amostra nova; depois a estrutura sai. Leitura fora de 0–100 conta como indisponível. O inteiro anunciado é o arredondamento da leitura ao ponto mais próximo.

**Rationale**: 10 segundos cabe nos 60 segundos da spec e evita regravar o rádio à toa. Dois minutos é o prazo da spec para largar um valor velho. Reescrever só na mudança reduz trabalho no mesmo rádio que atende o comando de LED.

**Alternatives considered**:

- Regravar a cada amostra, mesmo com o mesmo percentual. Rejeitado: trabalho de rádio sem mudança observável.
- Intervalo de anúncio mais curto que o default do NimBLE. Rejeitado: a carga muda devagar; encurtar o intervalo gasta energia sem ganho na spec.

## 5. Anúncio com sessão aberta

**Decision**: No `onConnect`, tentar `startAdvertising()` de novo. Se o controlador recusar, a sessão de LED segue e o `onDisconnect` existente volta a anunciar com o último pacote montado. O pacote é sempre reconstruído por um único método, para o reconnect não restaurar um anúncio sem o percentual novo.

**Rationale**: A spec pede o percentual no anúncio durante a sessão somente enquanto o rádio permitir os dois ao mesmo tempo. O ESP32 com uma conexão costuma aceitar anúncio concorrente; quando não aceita, degradar é o comportamento seguro da constituição.

**Alternatives considered**:

- Exigir notify na characteristic enquanto conectado. Rejeitado: canal de sessão está fora da spec.
- Desistir de anunciar assim que alguém conecta, sem tentar de novo. Rejeitado: a spec prefere manter o anúncio se o rádio deixar.

## 6. Isolamento da task

**Decision**: `PublishBatteryLevelAction` usa `IAction` e `TaskService`, prioridade 1, stack 4096, core 1. A task de LED permanece no core 0, prioridade 1. A action só chama o serviço de BLE para aplicar o pacote; não escreve na fila de LED.

**Rationale**: A constituição exige que I/O BLE crítico não bloqueie o efeito de LED. O core separado e a reescrita condicional atendem isso sem fila nova: o estado é um byte, não um fluxo.

**Alternatives considered**:

- Publicar dentro de `loop()` ou da task de LED. Rejeitado: acopla domínio no wiring e pode atrasar o efeito.
- Fila FreeRTOS só para um inteiro. Rejeitado: a action já é a dona do ciclo de 10 segundos; uma fila não muda o contrato.
