# UNIHIKER K10 - Demo Completo

Este é um projeto completo de demonstração para o **UNIHIKER K10**, implementando todas## 📋 Funcionalidades Implementadas
*Código desenvolvido em desenvolvimento assistido pelo GitHub Copilot usando Claude Sonnet 4 da Anthropic*


## �️ Como Baixar e Executar o Projeto

### Pré-requisitos
- **VSCode** i#### 💡 Sensor de Luz LTR303 (Nativo)
- **Classe**: `Adafruit_LTR303`
- **Funções principais**:
  - `ltr.begin()` - Inicializar sensor
  - `ltr.setGain(LTR3XX_GAIN_1)` - Configurar ganho
  - `ltr.setIntegrationTime(LTR3XX_INTEGTIME_50)` - Tempo de integração
  - `ltr.setMeasurementRate(LTR3XX_MEASRATE_50)` - Taxa de medição
  - `ltr.newDataAvailable()` - Verificar novos dados
  - `ltr.readBothChannels(visible_plus_ir, infrared)` - Ler canais de luz visível e infravermelha
- **PlatformIO Extension** instalada no VSCode
- **UNIHIKER K10** conectado via USB
- **Git** instalado (opcional, para clonar o repositório)

### 📥 Baixando o Código

#### Opção 1: Via Git (Recomendado)
```bash
git clone https://github.com/seu-usuario/UnihikerK10.git
cd UnihikerK10
```

#### Opção 2: Download ZIP
1. Clique em **"Code"** → **"Download ZIP"** na página do GitHub
2. Extraia o arquivo ZIP em uma pasta de sua escolha
3. Renomeie a pasta extraída para `UnihikerK10`

### 🔧 Configuração no VSCode

#### 1. Abrir o Projeto
1. Abra o **VSCode**
2. Vá em **File** → **Open Folder**
3. Selecione a pasta `UnihikerK10` onde você baixou o projeto
4. O VSCode deve detectar automaticamente que é um projeto PlatformIO

#### 2. Instalar Dependências
O PlatformIO irá automaticamente:
- Baixar as bibliotecas necessárias
- Configurar o toolchain ESP32-S3
- Preparar o ambiente de compilação

*Aguarde alguns minutos na primeira execução para download das dependências.*

#### 3. Conectar a Placa
1. Conecte o **UNIHIKER K10** ao computador via USB
2. Verifique se a placa aparece nas portas COM (Windows) ou tty (Linux/Mac)
3. O PlatformIO deve detectar automaticamente a placa

### 🚀 Compilar e Executar

#### Via Tasks do VSCode (Recomendado)
1. Pressione **Ctrl+Shift+P** para abrir o Command Palette
2. Digite "Tasks: Run Task" e pressione Enter
3. Escolha uma das opções:
   - **"PlatformIO Build"** - Apenas compilar (CTRL+ALT+B)
   - **"Build and Upload UNIHIKER K10 Demo"** - Compilar e fazer upload (CTRL+ALT+U)

#### Via PlatformIO Toolbar
1. Na barra inferior do VSCode, clique nos ícones:
   - **✓** (Build) - Compilar projeto
   - **→** (Upload) - Compilar e fazer upload
   - **🏠** (Home) - Abrir PlatformIO Home

#### Via Terminal
```bash
# Compilar apenas
platformio run

# Compilar e fazer upload
platformio run --target upload

# Limpar e recompilar
platformio run --target clean
platformio run --target upload
```

### 🔍 Monitoramento e Debug

#### Monitor Serial
1. Após o upload, abra o **Serial Monitor**:
   - Via toolbar: clique no ícone **📺**
   - Via comando: **Ctrl+Shift+P** → "PlatformIO: Serial Monitor"
2. Configure a velocidade para **115200 baud**
3. Observe os logs em tempo real dos sensores e funcionalidades

#### Logs Disponíveis
- Status de inicialização dos sensores
- Dados dos sensores em tempo real
- Feedback de botões pressionados
- Status de conexões WiFi/Bluetooth
- Informações de armazenamento SD

### 🐛 Solução de Problemas

#### Placa não detectada
1. Verifique se o cabo USB está conectado corretamente
2. Pressione o botão **RESET** na placa
3. Tente uma porta USB diferente
4. Instale os drivers USB-Serial se necessário

#### Erro de compilação
1. Limpe o projeto: `platformio run --target clean`
2. Exclua a pasta `.pio` e recompile
3. Verifique se todas as dependências foram baixadas

#### Upload falhou
1. Pressione **RESET** na placa antes do upload
2. Feche outros programas que podem estar usando a porta serial
3. Tente reduzir a velocidade de upload no `platformio.ini`

### 📋 Estrutura do Projeto
```
UnihikerK10/
├── platformio.ini     # Configurações do PlatformIO
├── src/
│   └── main.cpp      # Código principal
├── include/          # Headers customizados  
├── lib/             # Bibliotecas locais
└── README.md        # Este arquivo
```

## �📋 Funcionalidades Implementadas

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
  - **Interface visual otimizada** sem flicker
  - Controle por botões (A = trocar cor, B = piscar)ades disponíveis no dispositivo com exemplos práticos e interativos.


### 📊 2. Sensores (Modo 2)
- **Funcionalidade**: Leitura e exibição de todos os sensores disponíveis
- **Bibliotecas**: `unihiker_k10.h`, `Adafruit_LTR329_LTR303.h`, `AHT20.h` (biblioteca nativa)
- **Sensores implementados**:
  
  **🏃 Acelerômetro** (Nativo):
  - `k10.getAccelerometerX()` - Eixo X
  - `k10.getAccelerometerY()` - Eixo Y  
  - `k10.getAccelerometerZ()` - Eixo Z
  
  **🎤 Microfone** (Nativo):
  - `k10.readMICData()` - Dados do microfone
  
  **🌡️ Temperatura e Umidade** (Sensor Nativo AHT20):
  - `aht20.getData(AHT20::eAHT20TempC)` - Temperatura real em Celsius
  - `aht20.getData(AHT20::eAHT20HumiRH)` - Umidade relativa real
  - **Status**: Mostra "(AHT20)" para leituras reais ou "(sim)" para simulação
  
  **💡 Sensor de Luz** (Sensor Nativo LTR303):
  - `ltr.readBothChannels(visible_plus_ir, infrared)` - Leitura real de luz visível + infravermelha
  - `ltr.newDataAvailable()` - Verificar se há dados novos disponíveis
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
- **Funcionalidade**: Sistema de equalizador visual e reprodução de áudio
- **Bibliotecas**: `Music.h` (biblioteca nativa K10), `unihiker_k10.h`
- **Funções principais**:
  - `music.playMusic(RINGTONE)` - Reproduzir toque/campainha integrada
  - `music.stopPlayTone()` - Parar reprodução
  - `k10.readMICData()` - Ler dados reais do microfone para equalizador
- **Características do sistema**:
  - **Equalizador Visual**: 30 segundos de demonstração interativa
  - **LEDs reativos ao áudio**: Verde (baixo), Amarelo (médio), Vermelho (alto)
  - **Barra de equalização**: Visualização em tempo real "[===#### **]"
  - **Campainha final**: Toque automático após equalizador
- **Recursos**:
  - **Barra visual de volume do microfone** (dados reais em tempo real)
  - **Sistema de LEDs inteligente** com 3 níveis de intensidade
  - **Efeito strobe** ocasional em volumes altos
  - **Toque de campainha real** usando hardware nativo K10
  - **Reset automático** para loop contínuo de demonstração

### 📶 5. WiFi (Modo 5)
- **Funcionalidade**: Conectividade WiFi para rede local
- **Biblioteca**: `WiFi.h`
- **Funções principais**:
  - `WiFi.mode(WIFI_STA)` - Configurar modo Station
  - `WiFi.begin(ssid, senha)` - Conectar a rede WiFi
  - `WiFi.localIP()` - Obter IP local atribuído
  - `WiFi.RSSI()` - Verificar intensidade do sinal
  - `WiFi.scanNetworks()` - Escanear redes disponíveis
  - `WiFi.macAddress()` - Obter endereço MAC
- **Configurações**:
  - **SSID/Senha**: Configuráveis no código (variáveis `wifi_ssid` e `wifi_password`)
  - **Modo**: Station (conecta a rede existente)
  - **Reconexão automática**: Sistema inteligente de tentativas
- **Recursos**:
  - **Interface visual aprimorada** - sem flicker ou resíduos de tela
  - **Limpeza de tela otimizada** - transições suaves entre modos
  - **Monitoramento de sinal RSSI** com indicadores coloridos
  - **Scan automático de redes** a cada 20 segundos quando desconectado
  - **LEDs reativos ao sinal**: Verde (forte), Amarelo (médio), Vermelho (fraco)
  - **Informações completas**: IP, MAC, SSID, força do sinal
  - **Controles interativos**: Botão A (reconectar), Botão B (scan redes)

### 📱 6. Bluetooth (Modo 6) - Via Serial
- **Funcionalidade**: Comunicação de dados via Serial (debug)
- **Recursos**:
  - **Interface visual aprimorada** sem flicker
  - **Transições suaves** entre informações
  - Transmissão de dados dos sensores reais
  - Interface visual de status otimizada
  - Simulação de comunicação para desenvolvimento

### 💾 7. Cartão SD (Modo 7)
- **Funcionalidade**: Sistema completo de armazenamento em cartão SD
- **Bibliotecas**: `SD.h`, `FS.h`
- **Funções principais**:
  - `SD.begin()` - Inicializar cartão SD
  - `SD.open(arquivo, modo)` - Abrir arquivo (FILE_READ, FILE_WRITE, FILE_APPEND)
  - `SD.remove(arquivo)` - Deletar arquivo
  - **Funções customizadas implementadas**:
    - `writeToSD(filename, data)` - Escrever dados em arquivo
    - `readFromSD(filename)` - Ler conteúdo de arquivo
    - `appendToSD(filename, data)` - Anexar dados a arquivo existente
    - `deleteFromSD(filename)` - Deletar arquivo com logs
    - `listSDFiles()` - Listar todos os arquivos e pastas
    - `sdFileExists(filename)` - Verificar se arquivo existe
    - `getSDFileSize(filename)` - Obter tamanho do arquivo
- **Recursos**:
  - **Display otimizado** - eliminado flicker na interface
  - **Transições suaves** entre informações de status
  - **Teste automático** de escrita/leitura a cada 2 segundos
  - **Sistema de numeração sequencial** para arquivos de teste
  - **Informações detalhadas** - tamanho de arquivo, status de operações
  - **Reconexão automática** - tentativa de reconectar SD a cada 5s
  - **Logs detalhados** no Serial Monitor para debug

### 🎮 8. Botões (Modo 8)
- **Funcionalidade**: Interface com botões físicos
- **Biblioteca**: `unihiker_k10.h`
- **Funções principais**:
  - `k10.buttonA->isPressed()` - Status do botão A
  - `k10.buttonB->isPressed()` - Status do botão B  
  - `k10.buttonAB->isPressed()` - Status de A+B simultaneamente
- **Recursos**:
  - **Interface visual refinada** - sem flicker durante interações
  - **Feedback visual em tempo real** otimizado
  - Diferentes cores para cada botão
  - Sistema de debounce (200ms)
  - Controle de LEDs baseado nos botões
  - **Limpeza de tela inteligente** ao entrar no modo

### 🎨 9. Animação (Modo 9)
- **Funcionalidade**: Animações gráficas interativas
- **Biblioteca**: `unihiker_k10.h` (Canvas)
- **Funções principais**:
  - `k10.canvas->canvasCircle()` - Desenhar círculos
  - `k10.canvas->updateCanvas()` - Atualizar display
- **Recursos**:
  - **Fundo azul escuro personalizado** (0x000033)
  - **Sistema de animação otimizado** sem flicker
  - **Limpeza de tela única** ao iniciar o modo
  - Bola animada com física de colisão
  - Efeito de rastro (trail effect)
  - Controle de velocidade (Botão A)
  - Inversão de direção (Botão B)
  - Detecção de bordas automática

### 🎯 10. Demo Completo (Modo 0)
- **Funcionalidade**: Demonstração automática de todas as funcionalidades
- **Recursos**:
  - **Interface visual completamente otimizada** sem flicker
  - **Transições suaves** entre diferentes informações
  - Ciclo automático entre funcionalidades (2s cada)
  - LEDs arco-íris reativos aos sensores
  - Dados de sensores em tempo real
  - Gravação automática no SD
  - Transmissão simulada via Bluetooth/Serial
  - Feedback sonoro
  - **Rotação inteligente** de informações

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
- **Classe**: `AHT20` (biblioteca nativa do UNIHIKER K10)
- **Funções principais**:
  - `aht20.getData(AHT20::eAHT20TempC)` - Temperatura em Celsius
  - `aht20.getData(AHT20::eAHT20HumiRH)` - Umidade relativa (%)
- **Enumerações utilizadas**:
  - `AHT20::eAHT20TempC` - Constante para leitura de temperatura
  - `AHT20::eAHT20HumiRH` - Constante para leitura de umidade
- **Observação**: Não requer inicialização explícita (automática)

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

// Sistema de controle visual otimizado
static bool screenCleared;  // Flag local em cada função demonstrate*()
                            // Controla limpeza única da tela por modo
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

## 🚀 Compilação Rápida

### Comandos Básicos
```bash
# Compilar
platformio run

# Compilar e fazer upload
platformio run --target upload

# Limpar build
platformio run --target clean
```

### Tasks do VSCode
- **"PlatformIO Build"** - Compilar projeto
- **"Build and Upload UNIHIKER K10 Demo"** - Upload completo

> 💡 **Para instruções detalhadas de instalação e configuração**, consulte a seção [Como Baixar e Executar o Projeto](#-como-baixar-e-executar-o-projeto) no início deste README.

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
- **Armazenamento**: SD Card externo
- **Display**: TFT colorido com canvas

## 🎯 Características Especiais

### ✨ Sistema Visual Avançado
- **Eliminação completa de flicker**: Implementado padrão de limpeza de tela otimizado
- **Gestão inteligente de tela**: Cada modo limpa a tela apenas uma vez ao entrar
- **Flag de controle `screenCleared`**: Sistema que previne redesenhos desnecessários
- **Transições suaves**: Sem resíduos visuais entre diferentes modos
- **Performance otimizada**: Redução significativa de calls de `canvasClear()`
- **Compatibilidade com diferentes cores de fundo**: Suporte a fundos personalizados por modo

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
- **Sistema visual otimizado**: Limpeza de tela única por modo usando flag `screenCleared`
- **Redução de flicker**: Eliminação de redesenhos desnecessários da interface
- **Performance melhorada**: Menos calls de `canvasClear()` e operações de canvas
- Leitura otimizada de sensores (1Hz)
- Debounce de botões (300ms)
- Gestão automática de memória canvas
- **Reset inteligente**: Flag `screenCleared` resetada automaticamente ao trocar de modo

### Compatibilidade
- Suporta cartões SD de diferentes tamanhos
- Detecção automática de presença do cartão SD
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