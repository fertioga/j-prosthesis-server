# Implementation Plan: Percentual de bateria no anúncio

**Branch**: `001-battery-level-advertising` | **Date**: 2026-10-20 | **Spec**: [spec.md](./spec.md)

**Input**: Feature specification from `/specs/001-battery-level-advertising/spec.md`

**Note**: This template is filled in by the `/speckit-plan` command; its definition describes the execution workflow.

## Summary

A prótese passa a publicar o percentual inteiro de carga (0–100) no anúncio público Bluetooth, para um observador ler a bateria sem parear e sem abrir sessão. O byte segue o contrato em [contracts/battery-level-advertising.md](./contracts/battery-level-advertising.md): Service Data do Battery Service já declarado no firmware (`0x180F`), um byte, o mesmo significado da characteristic Battery Level.

A leitura elétrica não entra nesta entrega. `HardwareMapConf` só mapeia LED e o barramento do acelerômetro; não há pino, divisor nem fuel gauge. `BatteryLevelService::sample()` devolve carga indisponível, e o anúncio sai sem percentual numérico. Um flag de compilação, ausente do `platformio.ini`, injeta 0–100 só na validação de bancada descrita no [quickstart.md](./quickstart.md).

A task que publica roda fora da task de LED. O nome `J1-Prosthesis` permanece no pacote principal. O UUID de 128 bits do serviço de comando vai para o scan response, porque os dois não cabem juntos com o percentual nos 31 bytes do anúncio legado.

## Technical Context

**Language/Version**: C++ no estilo Arduino já usado em `src/` (headers `.hpp`, implementação `.cpp`)

**Primary Dependencies**: NimBLE-Arduino `^2.3.7` (já em `platformio.ini`). Nenhuma biblioteca nova.

**Storage**: N/A. O percentual não é telemetria de marcha e não vai para LittleFS.

**Testing**: Validação manual no hardware com escaneador BLE (LightBlue ou nRF Connect) e o fluxo de LED já descrito em `doc/led_codes.md`. Não há suíte de BLE no `test/` do PlatformIO. Cenários em [quickstart.md](./quickstart.md).

**Target Platform**: ESP32 DOIT DevKit V1, firmware `j-prosthesis-server`, anúncio BLE legado de 31 bytes

**Project Type**: Firmware embarcado (serviço + action FreeRTOS). O relógio é consumidor futuro do contrato e não é alterado neste repositório.

**Performance Goals**: Com leitura válida já disponível, o observador vê o percentual em até 30 segundos. Mudança de pelo menos 1 ponto aparece em até 60 segundos. A task amostra a cada 10 segundos e só reescreve o anúncio quando o byte, ou a presença dele, muda.

**Constraints**: Pacote de anúncio legado de 31 bytes. Nome do aparelho e descoberta para comando de luz permanecem. Sem percentual numérico quando a carga é inválida, inclusive o texto fixo `BAT:87`. Valor anunciado expira 2 minutos após a última leitura válida. A task de bateria não pode atrasar a task de LED. Sem PII no anúncio e sem log de paciente no Serial.

**Scale/Scope**: Um anunciante, um percentual, vários observadores passivos do mesmo pacote. Fora de escopo: tensão, carregamento, tempo restante, histórico, tela do relógio e leitura pela characteristic durante a sessão.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Princípio | Gate | Resultado |
|-----------|------|-----------|
| I. Propósito clínico | A feature serve para saber se a prótese aguenta a sessão antes de comandar luzes ou marcha | Pass |
| II. Tempo real | Publicação em task própria, core separado da task de LED, reescrita do anúncio só quando o valor muda. Falha de leitura omite o percentual e mantém LED e BLE de comando | Pass |
| III. Contrato BLE | Contrato versionado em `contracts/battery-level-advertising.md`, aditivo em relação ao comando de LED. O texto `BAT:87` não é contrato e não será ligado | Pass |
| IV. Offline-first | Percentual de carga não é registro clínico de marcha. Não persistir | Pass |
| V. Modular | `BleService` monta o anúncio. `BatteryLevelService` entrega a leitura. `PublishBatteryLevelAction` orquestra. `main.cpp` só instancia | Pass |
| VI. Telemetria clínica | Não mistura carga de bateria no registro de IMU | Pass |
| VII. YAGNI | Sem biblioteca nova, sem driver de gauge, sem pino inventado, sem schema em `config.json` | Pass |

**Revisão pós-Phase 1**: os artefatos de desenho mantêm os mesmos gates. O scan response carrega o UUID de comando que o relógio já procura; o pacote principal carrega o nome, que também basta para o relógio encontrar o aparelho. Nenhuma violação a justificar.

## Project Structure

### Documentation (this feature)

```text
specs/001-battery-level-advertising/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── battery-level-advertising.md
└── tasks.md             # /speckit-tasks — não criado aqui
```

### Source Code (repository root)

```text
src/
├── main.cpp                                          # wiring da action
└── app/
    ├── configs/
    │   └── BleConfig.hpp                             # UUIDs 0x180F / 0x2A19 já existentes
    ├── service/
    │   ├── bluetooth/ble/
    │   │   ├── BleService.hpp                        # applyAdvertisedBattery + rebuild
    │   │   └── BleService.cpp
    │   └── power/battery/
    │       ├── BatteryLevel.hpp                      # leitura válida ou indisponível
    │       └── BatteryLevelService.hpp               # sample(); injeção só com flag
    └── actions/
        ├── PublishBatteryLevelAction.hpp             # task FreeRTOS
        ├── PublishBatteryLevelAction.cpp
        └── sendBatteryLevel.cpp                      # removido: publicador rejeitado
```

**Structure Decision**: Firmware único do server, no layout service / action já usado pelo LED. A medição futura entra como outra implementação de `BatteryLevelService::sample()`, sem mudar o contrato do anúncio. O client-watch não é editado.

## Complexity Tracking

Nenhuma violação a justificar.
