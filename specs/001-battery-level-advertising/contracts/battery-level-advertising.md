# Contract: Percentual de bateria no anúncio

**Contract version**: `1.0.0`  
**Status**: aditivo em relação ao comando de LED  
**Device name**: `J1-Prosthesis` (inalterado)

Mudança incompatível neste layout exige versão MAJOR nova e atualização coordenada com o relógio. Esta versão não altera o payload de 5 bytes da characteristic de LED.

## O que o observador lê

Sem conexão e sem pareamento, o pacote principal de anúncio carrega a carga quando ela existe.

| AD type | Conteúdo | Quando |
|---------|----------|--------|
| `0x09` Complete Local Name | `J1-Prosthesis` | sempre |
| `0x16` Service Data, UUID 16 bits | UUID `0x180F` + 1 byte | somente com leitura válida |
| `0x03` Complete List of 16-bit UUIDs | `0x180F` | sempre |

O byte de Service Data é `uint8` little-endian de serviço:

| Byte | Faixa | Significado |
|------|-------|-------------|
| 0 | 0–100 | 0 = esgotada, 100 = completa |

Exemplo de 42%: AD `04 16 0F 18 2A` (length 4, type `0x16`, UUID `0x180F` em little-endian, valor 42).

Fora de 0–100 não é valor deste contrato. Carga desconhecida é a ausência do AD `0x16` de `0x180F`, não um sentinela.

## Scan response

| AD type | Conteúdo |
|---------|----------|
| `0x07` Complete List of 128-bit UUIDs | `12345678-1234-1234-1234-1234567890ab` |

Esse UUID é o serviço de comando já usado pelo relógio. Continua no scan response para o scan ativo que o relógio já faz. O nome no pacote principal continua suficiente para achar o aparelho.

## Fora deste contrato

- Characteristic Battery Level `0x2A19` (read/notify). Já existe no servidor e não é o canal desta feature. Um uso futuro dela deve publicar o mesmo inteiro 0–100, nunca a string `BAT:87`.
- Payload de LED, passkey e bonding. Permanecem como em `doc/led_codes.md`.
- Telas do relógio. O relógio ainda não parseia este Service Data; o layout acima é o que ele deve consumir quando essa tela existir.

## Observadores simultâneos

Todos leem o mesmo pacote. Não há percentual por conexão.
