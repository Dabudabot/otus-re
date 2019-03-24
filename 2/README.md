# Представление данных, кода, опкоды команд

## Задание

Неодходимо составить список как можно больших команд, которые будут иметь один и тот же смысл, но разные опкоды

## Результат

Смотри файл opcode.BIN

11C0                           adc          ax,ax   13C0                           adc          ax,ax 

10C0                           adc          al,al 12C0                           adc          al,al 

00C0                           add          al,al 02C0                           add          al,al 

01C0                           add          ax,ax 03C0                           add          ax,ax 

20C0                           and          al,al 22C0                           and          al,al 

21C0                           and          ax,ax 23C0                           and          ax,ax 

39C0                           cmp          ax,ax 3BC0                           cmp          ax,ax 

38C0                           cmp          al,al 3AC0                           cmp          al,al 

CC                             int          3     CD03                           int          3     

89C0                           mov          ax,ax 8BC0                           mov          ax,ax 

88C0                           mov          al,al 8AC0                           mov          al,al 

6690                           nop                90                             nop                

09C0                           or           ax,ax 0BC0                           or           ax,ax 

08C0                           or           al,al 0AC0                           or           al,al 

19C0                           sbb          ax,ax 1BC0                           sbb          ax,ax 

18C0                           sbb          al,al 1AC0                           sbb          al,al 

29C0                           sub          ax,ax 2BC0                           sub          ax,ax 

28C0                           sub          al,al 2AC0                           sub          al,al 

31C0                           xor          ax,ax 33C0                           xor          ax,ax 

30C0                           xor          al,al 32C0                           xor          al,al 

## Решение
1. Задача решена методом простого перебора команд взятых с этой вики: https://en.wikipedia.org/wiki/X86_instruction_listings

## Возможное дальнейшее развитие
1. Попробовать кинуть в бинарный файл простую последовательность от 0x0000 до 0xFFFF.
2. Открыть с помощью Hiew в режиме Decode.
3. Написать небольшой скрипт для поиска повторяющихся команд.

![Screenshot](img.PNG)