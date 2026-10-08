# J-Prosthesis Server Constitution

## Core Principles

### I. Propósito Clínico Primário
O firmware do server existe para apoiar fisioterapeutas e ortopedistas na
análise e correção da marcha de pacientes com amputação transtibial que
usam prótese. Toda feature MUST contribuir, direta ou indiretamente, para
controle do dispositivo, feedback ao usuário, telemetria de marcha ou
confiabilidade do sistema. Features sem vínculo clínico ou operacional
claro NÃO DEVEM ser aceitas.

**Rationale**: O valor do produto é clínico (correção de marcha e
performance de uso da prótese), não demonstração tecnológica.

### II. Confiabilidade em Tempo Real (NON-NEGOTIABLE)
O server roda acoplado à prótese e MUST priorizar comportamento
determinístico e estabilidade. Operações críticas (leitura IMU, efeitos
de LED, som de passo, I/O BLE) MUST usar FreeRTOS (tasks/queues) de forma
isolada, sem bloquear umas às outras indevidamente. Falha em um
subsistema MUST NOT derrubar o firmware inteiro quando for possível
degradar com segurança (ex.: LED ou som indisponível, telemetria
continua).

**Rationale**: Dispositivo vestível/acoplado; travamentos e jitter
comprometem uso e qualidade dos dados de marcha.

### III. Contrato BLE Bidirecional Explícito
A comunicação com o relógio (cliente) MUST ser bidirecional e baseada em
contratos versionados (UUIDs, payloads, schemas). Comandos do relógio
para o server (ex.: cor/efeito de LED, ativar/desativar som de passo) e
dados do server para o relógio (ex.: status, telemetria resumida, bateria)
MUST ser documentados e evoluir sem quebrar clientes sem bump de versão
do contrato. Payloads MUST ser parseáveis de forma determinística
(preferência: estruturas tipadas + JSON quando apropriado).

**Rationale**: Relógio e prótese evoluem em repositórios separados; o
contrato BLE é a API pública do server.

### IV. Offline-First e Integridade de Dados
Dados de telemetria MUST ser armazenados localmente (LittleFS ou
equivalente) até existir conectividade confiável para sync com a nuvem.
Perda de rede NÃO DEVE interromper aquisição. Persistência MUST incluir
metadados mínimos (timestamp, unidade/origem do sensor, versão do schema).
Upload eventual MUST ser idempotente e NÃO DEVE corromper ou duplicar
registros clínicos sem rastreabilidade.

**Rationale**: Uso clínico ocorre fora de Wi-Fi; dados de marcha são o
ativo principal do produto a longo prazo.

### V. Arquitetura Modular por Serviços
Hardware e efeitos (LED/NeoPixel, IMU/giroscópio-acelerômetro, som,
BLE, filesystem, RTOS) MUST ser encapsulados em serviços/actions com
interfaces claras. Novas funções MUST estender serviços existentes ou
criar novos isolados — NÃO acoplar lógica de domínio direto em `main`
ou em ISRs além do estritamente necessário.

**Rationale**: O código atual já segue services/actions/effects; a
constituição consolida isso como regra para escalar telemetria e
controle sem spaghetti.

### VI. Qualidade de Telemetria para Decisão Clínica
Sinais de marcha derivados de IMU MUST ser amostrados, rotulados e
exportados de forma que um profissional consiga interpretar (unidades
SI, eixos definidos, taxa de amostragem declarada, gaps detectáveis).
Processamento on-device (ex.: detecção de passo para som de robô) MUST
ser separável da telemetria bruta/derivada usada para análise — feedback
lúdico NÃO DEVE substituir nem adulterar o registro clínico.

**Rationale**: Fisioterapeutas/ortopedistas precisam de dados
auditáveis para correção de marcha; efeitos (LED/som) são secundários.

### VII. Simplicidade Embarcada (YAGNI)
O server MUST permanecer enxuto: ESP32 + Arduino/PlatformIO, NimBLE,
filesystem local e periféricos necessários. NÃO introduzir stacks,
middlewares ou abstrações “para o futuro” sem necessidade imediata
demonstrada por feature aprovada. Complexidade extra MUST ser justificada
em review (memória, CPU, energia, risco de regressão).

**Rationale**: Recursos finitos no MCU; over-engineering aumenta risco
em dispositivo clínico-assistivo.

## Platform & Scope Constraints

- Escopo desta constituição: **somente** `j-prosthesis-server` (firmware
  da prótese). O client-watch, backend cloud e dashboard são sistemas
  vizinhos; mudanças neles NÃO DEVEM ser feitas sob esta constituição,
  mas contratos compartilhados (BLE/schema) MUST permanecer coerentes.
- Stack canônica: PlatformIO, `espressif32`, board ESP32 DOIT DevKit V1
  (ou sucessor documentado), framework Arduino, FreeRTOS, LittleFS,
  NimBLE-Arduino, Adafruit NeoPixel, IMU (MPU6050/equivalente),
  ArduinoJson para config/telemetria estruturada.
- Funções de produto no server incluem, no mínimo: controle de LED
  (cor/efeitos), leitura/interpretação de giroscópio e acelerômetro,
  som de passo robótico (quando ativado), geração/persistência de
  telemetria de marcha, I/O BLE bidirecional.
- Sync cloud e dashboard estão fora do runtime crítico do server; o
  server MUST apenas preparar, armazenar e, quando houver caminho
  definido, entregar dados com schema estável.

## Clinical Data & Telemetry Integrity

- Dados de marcha são tratados como **dados sensíveis de saúde
  assistiva**: NÃO logar PII no Serial em builds de produção; identificadores
  de paciente, se existirem, MUST ser minimizados e documentados.
- Schemas de telemetria MUST ser versionados; breaking changes exigem
  bump MAJOR do schema e plano de migração/leitura retroativa.
- TODO(PRIVACY_POLICY): política completa de retenção, consentimento e
  anonimização será definida junto ao backend cloud.
- TODO(SAMPLING_RATES): taxas de amostragem IMU, buffering e limites de
  storage serão fixados na especificação da feature de telemetria.

## Governance

Esta constituição prevalece sobre convenções ad hoc no repositório
`j-prosthesis-server`. Amendments MUST: (1) atualizar este arquivo,
(2) bumpar `CONSTITUTION_VERSION` (MAJOR = remoção/redefinição
incompatível; MINOR = novo princípio/seção; PATCH = esclarecimento),
(3) registrar `Last Amended` em ISO-8601, (4) remover o Sync Impact
Report antes do commit final. PRs e reviews MUST verificar conformidade
com os princípios I–VII. Ambiguidades de implementação MUST ser
resolvidas via `/speckit-specify` (ou equivalente), não por bypass
silencioso da constituição.

**Version**: 1.0.0 | **Ratified**: 2026-10-08 | **Last Amended**: 2026-10-08
