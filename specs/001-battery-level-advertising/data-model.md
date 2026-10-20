# Data Model: Nível de bateria da prótese

Fonte da spec: entidade **Nível de bateria da prótese**. Sem persistência.

## BatteryLevel

Leitura aceita pelo aparelho antes de ir para o anúncio.

| Campo | Tipo | Regras |
|-------|------|--------|
| `valid` | bool | `true` somente com percentual inteiro de 0 a 100 |
| `percent` | inteiro 0–100 | Obrigatório quando `valid` é `true`. Ausente do anúncio quando `valid` é `false` |
| `sampledAtMs` | instante monotônico da amostra válida | Atualizado só quando entra uma amostra válida. Base do prazo de 2 minutos |

Não há identificador de paciente, tensão, flag de carregamento nem histórico.

### Validação

- Valor bruto abaixo de 0 ou acima de 100 → `valid = false`. O campo `percent` não é anunciado.
- Valor fracionário → inteiro mais próximo, com distância máxima de 1 ponto em relação à leitura aceita. 0 e 100 permanecem 0 e 100.
- Duas amostras com o mesmo inteiro → um único valor anunciado (FR-012).

### Transições

```text
[boot] --sem amostra válida--> Indisponível
Indisponível --amostra 0–100--> Anunciado(percent)
Anunciado(percent) --nova amostra com outro inteiro--> Anunciado(novo)
Anunciado(percent) --amostra inválida, dentro de 2 min da última válida--> Anunciado(percent)
Anunciado(percent) --2 min sem amostra válida--> Indisponível
Indisponível --amostra 0–100--> Anunciado(percent)
```

`Indisponível` significa pacote sem Service Data `0x180F`. `Anunciado` significa Service Data presente com aquele byte.

## Quem guarda o quê

| Dono | Estado | Relação |
|------|--------|---------|
| `BatteryLevelService` | resultado de `sample()` | Produz `BatteryLevel`. Na imagem padrão, sempre indisponível |
| `PublishBatteryLevelAction` | último `percent` anunciado, `sampledAtMs`, se o AD está no ar | Compara com a amostra nova e decide reescrever ou expirar |
| `BleService` | último pacote de anúncio e de scan response | Não interpreta carga. Aplica o byte ou a omissão que a action pedir |

A action observa o serviço de bateria e comanda o serviço de BLE. O serviço de BLE não lê ADC e o serviço de bateria não monta AD.
