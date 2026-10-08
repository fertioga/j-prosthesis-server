# Teste BLE dos LEDs com LightBlue

Guia para controlar os NeoPixels da prótese via Bluetooth LE usando o app **LightBlue** (iOS/Android).

## Pré-requisitos

- Firmware gravado no ESP32 (BLE + LED ativos)
- Cabo USB / alimentação na placa
- LED NeoPixel no **GPIO 16** (8 LEDs)
- App **LightBlue** instalado no celular
- Bluetooth ligado no celular

## Dispositivo

| Campo | Valor |
|-------|--------|
| Nome BLE | `J1-Prosthesis` |
| Passkey (se pedir) | `123456` |
| Bonding | ligado |
| MITM | desligado (facilita testes no LightBlue) |
| Escrita LED | exige conexão **criptografada** (`WRITE_ENC`) |

## UUIDs

### Serviço principal (custom)

| Tipo | UUID |
|------|------|
| Service | `12345678-1234-1234-1234-1234567890ab` |
| Characteristic LED (write) | `12345678-1234-1234-1234-1234567890ac` |

### Battery Service (padrão BLE, opcional neste teste)

| Tipo | UUID |
|------|------|
| Service | `0x180F` |
| Battery Level | `0x2A19` |

## Payload LED (5 bytes)

A characteristic LED espera **exatamente 5 bytes**, sem padding:

| Byte | Campo | Faixa | Descrição |
|------|--------|-------|-----------|
| 0 | `r` | 0–255 | Vermelho |
| 1 | `g` | 0–255 | Verde |
| 2 | `b` | 0–255 | Azul |
| 3 | `brightness` | 0–255 | Brilho global |
| 4 | `effect` | 0–4 | Código do efeito |

Struct no firmware: `LedPayloadBle` (`src/app/structs/LedPayloadBle.hpp`).

## Códigos de efeito (`effect`)

| Código | Efeito | Comportamento |
|--------|--------|----------------|
| `0` | TurnOff | Apaga todos os LEDs |
| `1` | TurnOn | Acende todos com a cor RGB |
| `2` | SOS | Pisca padrão SOS |
| `3` | Policy | Efeito Policy |
| `4` | Strobo | Estrobo |

## Exemplos em Hex (colar no LightBlue)

No LightBlue, use escrita em **Hex** (não texto ASCII).

| Ação | Hex (5 bytes) | Significado |
|------|----------------|-------------|
| Apagar | `00 00 00 00 00` | RGB 0, brilho 0, effect 0 |
| Vermelho full | `FF 00 00 FF 01` | R=255, brilho=255, TurnOn |
| Verde full | `00 FF 00 FF 01` | G=255, TurnOn |
| Azul full | `00 00 FF FF 01` | B=255, TurnOn |
| Branco médio | `FF FF FF 80 01` | RGB branco, brilho=128, TurnOn |
| SOS vermelho | `FF 00 00 FF 02` | effect 2 |
| Policy azul | `00 00 FF FF 03` | effect 3 |
| Strobo branco | `FF FF FF FF 04` | effect 4 |

Formato compacto (também válido): `FF0000FF01`

## Passo a passo no LightBlue

1. Ligue o ESP32 e aguarde o advertising (serial: `Iniciando advertising...`).
2. Abra o **LightBlue** → aba de scan / Peripherals.
3. Procure **`J1-Prosthesis`** e toque para conectar.
4. Se o app pedir pareamento/bonding, aceite. Se pedir PIN, use `123456`.
5. Abra o serviço `12345678-1234-1234-1234-1234567890ab`.
6. Selecione a characteristic `12345678-1234-1234-1234-1234567890ac` (Write).
7. Em **Write new value**, escolha formato **Hex**.
8. Digite exatamente **5 bytes**, ex.: `FF 00 00 FF 01`.
9. Envie (Write / Done). Os LEDs devem reagir.

### Dicas LightBlue

- Se a escrita falhar, desconecte, esqueça o dispositivo em Ajustes Bluetooth do celular, reconecte e tente de novo (a characteristic exige `WRITE_ENC`).
- Payload com tamanho ≠ 5 bytes é rejeitado (serial: `Invalid payload size`).
- Escreva **Hex**, não string tipo `"red"`.
- Mantenha o celular perto da placa na primeira conexão.

## Validação via Serial (opcional)

```bash
~/.platformio/penv/bin/pio device monitor -b 115200 --port /dev/cu.SLAB_USBtoUART
```

Logs úteis:

- `Iniciando advertising...` + endereço MAC → BLE OK
- `Cliente conectado.` → LightBlue conectou
- `Command sent to queue successfully.` → payload LED aceito
- `Invalid payload size...` → quantidade de bytes errada
- `Write bloqueado: conexão não criptografada.` → falta bonding/encryption

## Referência no código

- UUIDs / segurança: `src/app/configs/BleConfig.hpp`
- Callback de escrita: `src/app/service/bluetooth/ble/BleService.cpp`
- Protocolo e mapa de efeitos: `src/app/actions/ReadQueueLedBleAction.hpp`
- Struct do payload: `src/app/structs/LedPayloadBle.hpp`
