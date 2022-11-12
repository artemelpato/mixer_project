# MIXER 

## Что это такое?

Миксер создан для выделения сигнала в ядерно-физических экспериментов, используя 
технику _смешивания событий_.

## Как это работает 

Стандартный конструктор разбивает _центральность столкновения_ и _вершину_ на один класс, и 
ставит _глубину пулов_ на 5 (значение выбрано просто так :)). Есть нестандартный конструктор, 
который принимает на вход эти характеристики. 

Затем через методы _SetMixingType11/12/22_ выбирается тип смешивания (по типу частиц), 
по умолчанию -- 11.

**WORK IN PROGRESS**
## Возможности 

[x] Смешивание 11
[ ] Смешивание 12 и 22
[x] Сигнал
[ ] Фон
[ ] Разные папки и гистограммы для разных классов центральностей и вершины
[ ] Плоскость реакции
[ ] Выборй типа частиц
