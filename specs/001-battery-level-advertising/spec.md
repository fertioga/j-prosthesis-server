# Feature Specification: Percentual de bateria no anúncio

**Feature Branch**: `001-battery-level-advertising`

**Created**: 2026-10-20

**Status**: Draft

**Input**: User description: "passar a enviar o percentual da bateria via bluetooth usando o advertising"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Ver a carga sem conectar (Priority: P1)

Um fisioterapeuta, ou o relógio companheiro antes de iniciar uma sessão, aproxima-se da prótese ligada e lê o percentual de bateria no anúncio público que o aparelho já usa para ser encontrado.

**Why this priority**: A carga precisa ser visível antes de comandar luzes ou coletar marcha. Sem isso, a sessão começa sem saber se a prótese vai desligar.

**Independent Test**: Ligar a prótese com uma leitura de carga válida e, com um observador que só escuta anúncios, confirmar o percentual inteiro sem parear e sem abrir sessão.

**Acceptance Scenarios**:

1. **Given** a prótese ligada e com leitura de carga válida, **When** um observador escuta o anúncio público, **Then** o anúncio inclui um percentual inteiro de 0 a 100.
2. **Given** a prótese anunciando 42%, **When** o observador não pareia e não abre sessão, **Then** ainda assim ele obtém 42%.
3. **Given** o fluxo atual de achar a prótese pelo nome e enviar um comando de luz, **When** o percentual passa a constar do anúncio, **Then** esse fluxo continua concluindo com sucesso.

---

### User Story 2 - O anúncio acompanha a carga (Priority: P2)

Enquanto a prótese permanece ligada, o percentual anunciado acompanha a carga real. Quem observa de novo vê o valor novo, nos limites 0% e 100%.

**Why this priority**: Um número congelado leva a encerrar ou continuar a sessão com uma informação falsa.

**Independent Test**: Partir de uma carga conhecida, alterar a carga em pelo menos 1 ponto percentual e confirmar que um novo escutamento do anúncio mostra o valor novo, inclusive em 0% e 100%.

**Acceptance Scenarios**:

1. **Given** o anúncio mostrando um percentual válido, **When** a carga da prótese muda em pelo menos 1 ponto, **Then** um novo escutamento mostra o percentual novo em até 60 segundos.
2. **Given** a carga real em 0% ou em 100%, **When** um observador lê o anúncio, **Then** o valor anunciado é exatamente 0 ou 100.
3. **Given** dois observadores escutando ao mesmo tempo, **When** ambos leem o anúncio, **Then** os dois recebem o mesmo percentual.

---

### User Story 3 - Carga desconhecida não vira número falso (Priority: P3)

Se a prótese não consegue determinar a carga, o anúncio deixa de afirmar um percentual. Comandos de luz e o restante da operação continuam disponíveis.

**Why this priority**: O código atual prevê um exemplo fixo (`BAT:87`). Publicar isso, ou manter um valor velho por tempo indefinido, é pior do que omitir o número.

**Independent Test**: Impedir uma leitura válida na ligação e, depois, invalidar a leitura com a prótese já anunciando. Confirmar ausência de percentual numérico e sucesso de um comando de luz.

**Acceptance Scenarios**:

1. **Given** a prótese ligada sem leitura válida, **When** um observador escuta o anúncio, **Then** não há percentual numérico de bateria.
2. **Given** um percentual válido já anunciado, **When** novas leituras válidas deixam de existir por 2 minutos, **Then** o anúncio deixa de incluir percentual numérico.
3. **Given** a carga indisponível, **When** um cliente já pareado envia um comando de luz, **Then** o comando continua sendo aceito.

### Edge Cases

- Leitura ainda indisponível nos primeiros segundos após ligar: nenhum percentual numérico até existir uma leitura válida.
- Variação menor que 1 ponto percentual: o inteiro anunciado pode permanecer o mesmo.
- Valor de leitura fora de 0–100: o anúncio não publica esse valor; a carga fica indisponível até haver uma leitura dentro da faixa.
- Sessão já aberta com o relógio: o percentual no anúncio continua enquanto o rádio permitir anunciar junto com a sessão. Esta feature não exige um segundo canal durante a sessão.
- Retomada depois de uma falha de leitura: o próximo percentual válido volta ao anúncio em até 60 segundos.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: O anúncio público da prótese MUST incluir o percentual de carga quando existir uma leitura válida.
- **FR-002**: O percentual MUST ser um inteiro de 0 a 100, em que 0 é bateria esgotada e 100 é carga completa.
- **FR-003**: Um observador MUST obter esse percentual sem parear e sem abrir sessão.
- **FR-004**: O valor anunciado MUST ser o mesmo percentual de carga da prótese, arredondado ao inteiro mais próximo, com erro máximo de 1 ponto em relação à leitura aceita pelo aparelho.
- **FR-005**: Após uma mudança de pelo menos 1 ponto, o anúncio MUST refletir o novo percentual em até 60 segundos.
- **FR-006**: Os limites 0 e 100 MUST ser anunciados como exatamente esses valores.
- **FR-007**: Sem leitura válida, o anúncio MUST NOT incluir percentual numérico, inclusive MUST NOT reutilizar um valor de demonstração.
- **FR-008**: Se leituras válidas cessarem, o anúncio MUST deixar de afirmar percentual numérico após 2 minutos.
- **FR-009**: A inclusão do percentual MUST preservar o nome atual do aparelho e a capacidade de encontrá-lo e de comandar as luzes como hoje.
- **FR-010**: A atualização do percentual MUST NOT interromper comandos de luz nem o restante da operação da prótese.
- **FR-011**: O anúncio MUST incluir somente o percentual de carga da prótese, sem identificador de paciente ou outro dado clínico.
- **FR-012**: Observadores simultâneos MUST receber o mesmo percentual para a mesma leitura.

### Key Entities

- **Nível de bateria da prótese**: percentual inteiro de 0 a 100, momento da leitura e estado válido ou indisponível. Representa a carga do aparelho, sem vínculo com paciente.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Com a prótese ligada e uma leitura válida já disponível, um observador vê o percentual em até 30 segundos, sem abrir sessão.
- **SC-002**: Em 100% das leituras válidas conferidas, o valor anunciado coincide com a carga da prótese dentro de 1 ponto percentual.
- **SC-003**: Depois de uma mudança de pelo menos 1 ponto, o valor novo aparece para o observador em até 60 segundos em pelo menos 95% das repetições.
- **SC-004**: Nos casos de 0% e 100%, o valor anunciado é exatamente o limite em 100% das conferências.
- **SC-005**: Com a carga indisponível, 100% dos escutamentos ficam sem percentual numérico, e um comando de luz do fluxo atual continua concluindo com sucesso.
- **SC-006**: O fluxo atual de encontrar a prótese pelo nome e comandar as luzes continua concluindo com sucesso em 100% das repetições deste teste.

## Assumptions

- O percentual é a carga da bateria da prótese. Tensão, estado de carregamento, tempo restante e histórico ficam fora desta feature. O firmware ainda não mede essa carga; o planejamento precisa definir de onde vem a leitura. Enquanto não houver leitura válida, vale FR-007.
- O consumidor previsto é quem já escuta o anúncio da prótese, em especial o relógio companheiro antes da sessão. Alterar telas do relógio fica fora deste repositório. O significado do percentual tem de ser estável para esse cliente.
- A leitura durante uma sessão já aberta fica fora desta feature. Se os dois canais existirem depois, os dois devem expressar o mesmo percentual.
- Publicar só o percentual no anúncio é aceitável: não é identificador de paciente. O nome do aparelho permanece o atual.
- Durante uma sessão ativa, manter o percentual no anúncio é o comportamento desejado enquanto o rádio permitir anunciar ao mesmo tempo.
- A atualização do anúncio não pode atrasar comandos de luz nem a operação já existente da prótese.
- O contrato deste percentual, quando evoluir de forma incompatível, exige versão nova coordenada com o relógio.
