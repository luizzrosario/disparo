# Disparo

Este projeto controla LEDs usando um botão na placa BitDogLab. Quando o botão é pressionado, os LEDs acendem e se apagam em sequência após um tempo determinado.

## 📌 Pinos Utilizados
| Componente  | Pino |
|------------|------|
| LED Verde  | GP11 |
| LED Azul   | GP12 |
| LED Vermelho | GP13 |
| Botão      | GP5  |

## 🚀 Como Usar
1. Conecte os componentes conforme a tabela acima.
2. Compile e carregue o código na BitDogLab.
3. Pressione o botão para ativar os LEDs.
4. Os LEDs desligam automaticamente em sequência.

## ⏳ Configuração de Tempo
- `DEBOUNCE_TIME = 200ms`: Previne acionamentos indesejados do botão.
- `DELAY_TIME = 3000ms`: Tempo para desligamento dos LEDs.

## ⏲️ Temporizadores e Callback
Este projeto utiliza temporizadores para gerenciar o acionamento dos LEDs de forma automática. Quando o botão é pressionado, um callback é acionado para acender os LEDs e iniciar um temporizador que gerencia o desligamento progressivo.

- **Callback do botão:** Detecta a pressão do botão, ativa os LEDs e inicia o temporizador.
- **Temporizador:** Garante o desligamento progressivo dos LEDs, acionando callbacks em intervalos pré-definidos.
- **Callback dos LEDs:** Desliga cada LED em sequência, garantindo um efeito de desligamento gradual.