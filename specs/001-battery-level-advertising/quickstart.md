# Quickstart: Percentual de bateria no anúncio

Validação de bancada no ESP32. O formato esperado está em [contracts/battery-level-advertising.md](./contracts/battery-level-advertising.md). Os estados estão em [data-model.md](./data-model.md).

A imagem padrão não mede célula: `sample()` volta indisponível. Os cenários com 0, 42 e 100 usam o flag local `BATTERY_LEVEL_INJECT`. Esse flag não entra no `platformio.ini` versionado.

## Pré-requisitos

- Firmware desta feature gravado na placa DOIT DevKit V1
- LED no GPIO 16, como em `doc/led_codes.md`
- Celular com LightBlue ou nRF Connect, Bluetooth ligado
- Serial em 115200
- Para os cenários de percentual: build local com `-DBATTERY_LEVEL_INJECT` (não gravar esse flag no `platformio.ini`). No Serial, uma linha por vez: `BAT 42`, `BAT 0`, `BAT 100` ou `BAT OFF`

## Cenário 1 — Sem leitura, sem número

1. Gravar a imagem **sem** `BATTERY_LEVEL_INJECT`.
2. Ligar a placa e esperar o Serial mostrar o advertising e o endereço.
3. Escanear sem conectar.

**Esperado**: nome `J1-Prosthesis`. Nenhum Service Data `0x180F` com byte de percentual. UUID `0x180F` pode aparecer na lista de serviços.

## Cenário 2 — Percentual sem sessão

1. Gravar com `BATTERY_LEVEL_INJECT` e injetar `42` antes de olhar o anúncio.
2. Escanear sem parear e sem conectar.
3. Repetir o escaneamento em até 30 segundos.

**Esperado**: Service Data `04 16 0F 18 2A`. Os dois escaneamentos mostram 42.

## Cenário 3 — Limites e troca

1. Injetar `0`. Esperar até 60 segundos e ler o anúncio.
2. Injetar `100`. Esperar até 60 segundos e ler de novo.
3. Injetar `42` outra vez.

**Esperado**: byte `0x00`, depois `0x64`, depois `0x2A`. Nenhum outro valor nesses três passos.

## Cenário 4 — Expiração

1. Com `42` no ar, parar de injetar amostras válidas (injeção inválida, ou fora de 0–100).
2. Antes de 2 minutos, escanear.
3. Depois de 2 minutos, escanear de novo.

**Esperado**: o primeiro escaneamento ainda mostra 42. O segundo não traz Service Data de percentual.

## Cenário 5 — Luzes continuam

1. No estado do cenário 1 (sem percentual) e de novo no cenário 2 (com 42).
2. Conectar, parear se pedido (passkey `123456`) e escrever o payload de LED de `doc/led_codes.md`.

**Esperado**: o efeito de LED ocorre nas duas situações. O nome segue visível para um novo escaneamento depois de desconectar.

## Cenário 6 — Descoberta do relógio

1. Com o anúncio no ar, confirmar no escaneador ativo o nome no pacote principal e o UUID `12345678-1234-1234-1234-1234567890ab` no scan response.

**Esperado**: os dois identificadores que `WatchBleClient` já aceita continuam presentes. Nenhuma tela do relógio precisa mudar neste teste.

## Limite conhecido

Sem `BATTERY_LEVEL_INJECT`, a placa atual não produz 0–100, porque não há sensor de bateria no mapa de hardware. Esse quickstart prova o anúncio e a omissão. A conferência contra uma célula real fica para a feature que definir pino e curva.
