# UNIHIKER K10 - Demo Completo

Este é um projeto completo de demonstração para o **UNIHIKER K10**, implementando todas as funcionalidades disponíveis no dispositivo com exemplos práticos e interativos.

## 📋 Funcionalidades Implementadas

### 🌈 1. LEDs RGB (Modo 1)
- **Funcionalidade**: Controle completo dos 3 LEDs RGB integrados
- **Biblioteca**: `unihiker_k10.h` 
- **Funções principais**:
  - `k10.rgb->begin()` - Inicializar LEDs
  - `k10.rgb->write(led, r, g, b)` - Definir cor individual
  - `k10.rgb->show()` - Atualizar display dos LEDs
  - `k10.rgb->brightness(nivel)` - Controlar brilho (0-9)
- **Recursos**:
  - 7 cores pré-definidas (Vermelho, Verde, Azul, Amarelo, Magenta, Ciano, Branco)
  - Efeito arco-íris rotativo
  - **Auto-ajuste de brilho** baseado no sensor de luz real
  - Controle por botões (A = trocar cor, B = piscar)

### 📊 2. Sensores (Modo 2)
- **Funcionalidade**: Leitura e exibição de todos os sensores disponíveis
- **Bibliotecas**: `unihiker_k10.h`, `Adafruit_LTR329_LTR303.h`, `DFRobot_AHT20.h`
- **Sensores implementados**:
  
  **🏃 Acelerômetro** (Nativo):
  - `k10.getAccelerometerX()` - Eixo X
  - `k10.getAccelerometerY()` - Eixo Y  
  - `k10.getAccelerometerZ()` - Eixo Z
  
  **🎤 Microfone** (Nativo):
  - `k10.readMICData()` - Dados do microfone
  
  **🌡️ Temperatura e Umidade** (Sensor Nativo AHT20):
  - `aht20.getTemperature_C()` - Temperatura real em Celsius
  - `aht20.getHumidity_RH()` - Umidade relativa real
  - **Status**: Mostra "(AHT20)" para leituras reais ou "(sim)" para simulação
  
  **💡 Sensor de Luz** (Sensor Nativo LTR303):
  - `ltr.readBothChannels()` - Leitura real de luz visível + infravermelha
  - Range dinâmico auto-calibrável (min/max)
  - Porcentagem de luminosidade (0-100%)
  - **Status**: Mostra "85% (1456)" para leituras reais ou "2048 (sim)" para simulação
  
- **Recursos**:
  - Atualização em tempo real (1 segundo)
  - **Sensores físicos reais**: Acelerômetro, microfone, AHT20 (temp/umidade), LTR303 (luz)
  - **Fallback inteligente**: Simulação baseada em sensores reais se algum sensor falhar
  - **Auto-calibração**: Sensor de luz adapta range automaticamente
  - Indicadores visuais inteligentes com alertas contextuais

### 📷 3. Câmera/AI (Modo 3)
- **Funcionalidade**: Sistema de detecção baseado em sensores físicos reais
- **Biblioteca**: `AIRecognition.h`
- **Funções principais**:
  - `ai.initAi()` - Inicializar sistema AI
  - `k10.getAccelerometerX/Y/Z()` - Detectar movimento
  - `k10.readMICData()` - Detectar atividade sonora
- **Recursos baseados em sensores reais**:
  - **Detecção de movimento** via acelerômetro
  - **Detecção de atividade** via microfone
  - **Feedback inteligente** baseado em ambiente
  - **Alertas visuais** para diferentes tipos de detecção

### 🔊 4. Som/Áudio (Modo 4)
- **Funcionalidade**: Sistema de áudio com reconhecimento de voz e feedback visual
- **Bibliotecas**: `asr.h` (Automatic Speech Recognition), `unihiker_k10.h`
- **Funções principais**:
  - `asr.asrInit(CONTINUOUS, CN_MODE, 6000)` - Inicializar ASR
  - `asr.addASRCommand(id, comando)` - Adicionar comando
  - `asr.isWakeUp()` - Verificar se está ativo
  - `asr.isDetectCmdID(id)` - Detectar comando específico
  - `k10.readMICData()` - Ler dados reais do microfone
  - `playTone(frequencia, duracao)` - Feedback visual sincronizado
- **Comandos de voz**:
  - "Jarvis" - Despertar o sistema
  - "Teste um" - Ligar LEDs
  - "Teste dois" - Desligar LEDs
- **Recursos**:
  - Barra visual de volume do microfone (dados reais)
  - Feedback visual avançado com padrões de LEDs
  - Sons simulados com diferentes frequências e padrões
  - Sincronização visual-sonora realista

### 📶 5. WiFi (Modo 5)
- **Funcionalidade**: Conectividade WiFi em modo Access Point
- **Biblioteca**: `WiFi.h`
- **Funções principais**:
  - `WiFi.softAP(ssid, senha)` - Criar Access Point
  - `WiFi.softAPIP()` - Obter IP do AP
  - `WiFi.softAPgetStationNum()` - Contar clientes conectados
- **Configurações**:
  - **SSID**: K10-Demo
  - **Senha**: 12345678
  - **Modo**: Access Point
- **Recursos**:
  - Monitoramento de clientes conectados
  - Indicadores visuais de atividade TX/RX
  - Informações de rede em tempo real

### 📱 6. Bluetooth (Modo 6) - Via Serial
- **Funcionalidade**: Comunicação de dados via Serial (debug)
- **Recursos**:
  - Transmissão de dados dos sensores reais
  - Interface visual de status
  - Simulação de comunicação para desenvolvimento

### 💾 7. Cartão SD (Modo 7)
- **Funcionalidade**: Armazenamento em cartão SD e memória interna
- **Bibliotecas**: `SD.h`, `LittleFS.h`, `FS.h`
- **Funções principais**:
  - `SD.begin()` - Inicializar cartão SD
  - `SD.open(arquivo, modo)` - Abrir arquivo
  - `LittleFS.begin()` - Inicializar sistema de arquivos interno
- **Recursos**:
  - Teste de escrita e leitura
  - Informações de capacidade e uso
  - Log automático de eventos
  - Fallback para LittleFS se SD falhar

### 🎮 8. Botões (Modo 8)
- **Funcionalidade**: Interface com botões físicos
- **Biblioteca**: `unihiker_k10.h`
- **Funções principais**:
  - `k10.buttonA->isPressed()` - Status do botão A
  - `k10.buttonB->isPressed()` - Status do botão B  
  - `k10.buttonAB->isPressed()` - Status de A+B simultaneamente
- **Recursos**:
  - Feedback visual em tempo real
  - Diferentes cores para cada botão
  - Sistema de debounce (200ms)
  - Controle de LEDs baseado nos botões

### 🎨 9. Animação (Modo 9)
- **Funcionalidade**: Animações gráficas interativas
- **Biblioteca**: `unihiker_k10.h` (Canvas)
- **Funções principais**:
  - `k10.canvas->canvasCircle()` - Desenhar círculos
  - `k10.canvas->updateCanvas()` - Atualizar display
- **Recursos**:
  - Bola animada com física de colisão
  - Efeito de rastro (trail effect)
  - Controle de velocidade (Botão A)
  - Inversão de direção (Botão B)
  - Detecção de bordas automática

### 🎯 10. Demo Completo (Modo 0)
- **Funcionalidade**: Demonstração automática de todas as funcionalidades
- **Recursos**:
  - Ciclo automático entre funcionalidades (3s cada)
  - LEDs arco-íris
  - Dados de sensores em tempo real
  - Gravação automática no SD
  - Transmissão simulada via Bluetooth/Serial
  - Feedback sonoro

## 🎮 Como Usar

### Navegação por Menus
1. **A + B**: Navegar entre modos (0-10)
2. **Botão A**: Ação específica do modo atual
3. **Botão B**: Ação alternativa do modo atual

### Controles por Modo

| Modo | Botão A | Botão B |
|------|---------|---------|
| **1 - LEDs** | Trocar cor | Piscar LEDs |
| **2 - Sensores** | - | - |
| **3 - Câmera** | - | - |
| **4 - Áudio** | Beep simples | Tom complexo |
| **5 - WiFi** | - | - |
| **6 - Bluetooth** | - | - |
| **7 - SD Card** | - | - |
| **8 - Botões** | Teste A | Teste B |
| **9 - Animação** | +Velocidade | Inverter direção |
| **0 - Demo** | - | - |

## 🛠️ Configuração do Projeto

### Dependências (platformio.ini)
```ini
[env:unihiker]
platform = https://github.com/DFRobot/platform-unihiker.git
board = unihiker_k10
framework = arduino

lib_deps = 
    unihiker_k10
    adafruit/Adafruit LTR329 and LTR303
    WiFi
    FS
    SD
    LittleFS
    AIRecognition
    asr
```

### Bibliotecas Principais Utilizadas

#### 🔧 unihiker_k10.h
- **Classe**: `UNIHIKER_K10`
- **Funções principais**:
  - `k10.begin()` - Inicialização geral
  - `k10.initScreen(orientacao)` - Configurar tela
  - `k10.creatCanvas()` - Criar canvas para desenho
  - `k10.setScreenBackground(cor)` - Cor de fundo

#### 🎨 Canvas (Desenho na Tela)
- `k10.canvas->canvasClear()` - Limpar tela
- `k10.canvas->canvasText(texto, x, y, cor)` - Texto
- `k10.canvas->canvasCircle(x, y, raio, cor_borda, cor_preench, preenchido)` - Círculo
- `k10.canvas->updateCanvas()` - Atualizar display

#### � Sensor de Luz LTR303 (Nativo)
- **Classe**: `Adafruit_LTR303`
- **Funções principais**:
  - `ltr.begin()` - Inicializar sensor
  - `ltr.setGain(LTR3XX_GAIN_1)` - Configurar ganho
  - `ltr.setIntegrationTime(LTR3XX_INTEGTIME_50)` - Tempo de integração
  - `ltr.newDataAvailable()` - Verificar novos dados
  - `ltr.readBothChannels(visible, infrared)` - Ler canais de luz

#### 🌡️ Sensor AHT20 (Nativo)
- **Classe**: `DFRobot_AHT20`
- **Funções principais**:
  - `aht20.begin()` - Inicializar sensor
  - `aht20.getTemperature_C()` - Temperatura em Celsius
  - `aht20.getHumidity_RH()` - Umidade relativa (%)

#### �🎤 asr.h (Reconhecimento de Voz)
- **Classe**: `ASR`
- **Modos**: `CONTINUOUS`, `CN_MODE` (Chinês), `EN_MODE` (Inglês)
- **Comandos configurados**:
  - ID 1: "kai deng" (ligar)
  - ID 2: "guan deng" (desligar)

## 📝 Estrutura do Código

### Variáveis Globais
```cpp
UNIHIKER_K10 k10;           // Objeto principal do dispositivo
ASR asr;                    // Reconhecimento de voz
AIRecognition ai;           // Inteligência artificial
int currentMode = 0;        // Modo atual (0-10)
bool modeChanged = true;    // Flag de mudança de modo
```

### Dados dos Sensores
```cpp
float temperature = 0.0;      // Temperatura (°C) - AHT20 real
float humidity = 0.0;         // Umidade (%) - AHT20 real
uint16_t lightLevel = 0;      // Nível de luz (compatibilidade)
uint16_t lightRaw = 0;        // Valor bruto LTR303
uint16_t lightPercent = 0;    // Porcentagem de luz (0-100%)
float accelX, accelY, accelZ; // Acelerômetro (g)
uint64_t micData = 0;         // Dados do microfone

// Controles dos sensores nativos
bool lightSensorAvailable = false;     // Status LTR303
bool tempHumiSensorAvailable = false;  // Status AHT20
```

### Funções Principais
- `setup()` - Inicialização do sistema
- `loop()` - Loop principal
- `readAllSensors()` - Leitura de todos os sensores
- `handleButtons()` - Processamento dos botões
- `demonstrate...()` - Funções de demonstração para cada modo

## 🚀 Compilação e Upload

### Usando PlatformIO
```bash
# Compilar
platformio run

# Compilar e fazer upload
platformio run --target upload

# Limpar build
platformio run --target clean
```

### Usando VS Code com PlatformIO
1. Usar a task **"PlatformIO Build"** para compilar
2. Usar a task **"Build and Upload UNIHIKER K10 Demo"** para upload

## 📊 Uso de Memória
- **RAM**: 23.2% (76,028 bytes / 327,680 bytes)
- **Flash**: 88.1% (4,617,741 bytes / 5,242,880 bytes)

## 🔧 Configurações Técnicas

### Hardware
- **Plataforma**: ESP32-S3
- **Frequência**: 240MHz
- **RAM**: 320KB
- **Flash**: 16MB
- **Tela**: 240x320 pixels
- **LEDs**: 3x RGB endereçáveis

### Pinout e Recursos
- **Botões**: A, B e A+B (combinado)
- **Sensores**: Acelerômetro interno, microfone
- **Conectividade**: WiFi 2.4GHz
- **Armazenamento**: SD Card + LittleFS interno
- **Display**: TFT colorido com canvas

## 🎯 Características Especiais

### Sistema de Sensores Nativos Reais
- **AHT20**: Sensor de temperatura e umidade de precisão
- **LTR303**: Sensor de luz visível + infravermelha
- **Fallback inteligente**: Simulação baseada em outros sensores se algum falhar
- **Auto-calibração**: Range dinâmico para sensor de luz
- **Interface clara**: Indicação visual de dados reais vs simulados

### Sistema de Menu Intuitivo
- Interface visual clara com cores diferenciadas
- Navegação simples com botões físicos
- Feedback visual e sonoro para todas as ações

### Gestão de Recursos
- Leitura otimizada de sensores (1Hz)
- Debounce de botões (200ms)
- Gestão automática de memória canvas

### Compatibilidade
- Suporta cartões SD de diferentes tamanhos
- Fallback automático para LittleFS
- Simulação de funcionalidades não disponíveis

## 🐛 Troubleshooting

### Problemas Comuns
1. **SD Card não reconhecido**: Verificar formato (FAT32) e conexão
2. **WiFi não funciona**: Verificar se está no modo AP correto
3. **LEDs não acendem**: Verificar inicialização com `k10.rgb->begin()`
4. **Tela em branco**: Verificar orientação da tela (`screen_dir`)
5. **Sensor de luz falha**: Verificar se LTR303 está conectado/funcionando - fallback para simulação ativo
6. **Sensor temp/umidade falha**: Verificar se AHT20 está funcionando - fallback para simulação ativo
7. **Dados simulados**: Se sensores mostram "(sim)", significa que está usando fallback - sensores nativos podem estar com problemas

### Debug
- Monitor Serial (115200 baud) para logs detalhados dos sensores
- Formato do log: `T:24.2°C H:58.3% L:85% (1456 raw) AccelX:0.12 AccelY:-0.03 AccelZ:0.98`
- LEDs como indicadores de status
- Mensagens na tela para feedback visual
- Status dos sensores nativos mostrado durante inicialização

## 📄 Licença

Este projeto é fornecido como exemplo educacional para o UNIHIKER K10.

---

**Desenvolvido para UNIHIKER K10** | **ESP32-S3** | **Sensores Nativos AHT20 + LTR303** | **PlatformIO** | **Arduino Framework**