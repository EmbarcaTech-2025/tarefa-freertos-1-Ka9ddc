
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Ryan Micael Benício dos Santos**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 13 de Junho de 2025

---

## Sistema Multitarefa com FreeRTOS na BitDogLab
Este projeto foi desenvolvido com o objetivo de aplicar conceitos de Sistemas Operacionais de Tempo Real (RTOS) utilizando o FreeRTOS na BitDogLab

O sistema controla três periféricos de forma concorrente, demonstrando a criação e gerenciamento de múltiplas tarefas, escalonamento e o uso de funções para suspender e retomar tarefas.

Funcionalidade
O projeto implementa três tarefas principais que executam de forma concorrente:

Tarefa do LED RGB que controla o LED RGB da placa, alternando ciclicamente entre as cores vermelho, verde e azul, onde cada cor permanece acesa por 500ms.

Tarefa do Buzzer que controla o buzzer da placa, emitindo um bipe curto de 100ms a cada 1segundo.

Tarefa dos Botões que faz a leitura dos dois botões da placa através de polling a cada 100ms, possui a prioridade mais alta para garantir uma resposta rápida à interação do usuário. Ao pressionar o botão A, a tarefa dos LEDs é suspensa, ao pressionar o botão B a tarefa do buzzer é suspensa, ao soltar os botões a execução das tarefas são retomadas.

## Reflexões sobre o Sistema
Essa seção contém algumas reflexões feitas sobre o desenvolvimento dessa aplicação, voltadas para o funcionamento do RTOS.

### 1. O que acontece se todas as tarefas tiverem a mesma prioridade?

Quando todas as tarefas têm a mesma prioridade, o FreeRTOS usa um sistema chamado **Round-Robin**, que divide o tempo de uso da CPU igualmente entre elas. Cada tarefa executa por um pequeno período e depois dá lugar para a próxima.

Esse processo acontece de forma tão rápida que parece que todas as tarefas estão rodando ao mesmo tempo. Caso alguma tarefa precise esperar (por exemplo, usando `vTaskDelay()`), ela sai da fila temporariamente, e outra pode usar a CPU antes do tempo acabar.


### 2. Qual tarefa consome mais tempo de CPU?

A tarefa `button_task` é a que mais consome CPU no projeto, mesmo sendo uma operação mais simples. Isso acontece porque ela roda com mais frequência, a cada **100 ms**, ou seja, **10 vezes por segundo**.

Em comparação, as tarefas `led_task` e `buzzer_task` ficam mais tempo "dormindo", aguardando períodos maiores (como 500 ms ou mais), então acabam usando bem menos a CPU.

### 3. Quais os riscos de usar polling sem prioridades adequadas?

Utilizar **polling** sem organizar corretamente as prioridades pode trazer alguns problemas que acabam com o sentido do RTOS:

- **Demora na resposta:**  
  Se a `button_task` tiver prioridade baixa, ela pode demorar para ser executada, fazendo com que o sistema leve tempo para reagir quando o botão for pressionado.

- **Tarefas menos importantes interferindo:**  
  Tarefas que não são urgentes, como piscar um LED, podem acabar rodando mais do que deveriam, ocupando a CPU e atrasando as tarefas importantes.

- **Perda de eventos:**  
  Em casos mais graves, tarefas mais prioritárias podem ocupar tanto tempo da CPU que a `button_task` nunca é executada, e o sistema perde o evento do botão sendo pressionado.

---

## 📜 Licença
GNU GPL-3.0.
