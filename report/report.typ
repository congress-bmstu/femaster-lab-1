#import "utils.typ": notes
#import "utils-environments.typ" : *
#import "titlepage.typ": *

#show: notes.with(
  author: "",
  title: "#title"
)

#titlepage(
  "report",
  ["Фундаментальные науки"],
  ["Вычислительная математика и математическая физика" (ФН-11)],
  "Расчёт конструкций из композитов методом конечных элементов",
  "лабораторной работе №1",
  "Безвихревое течение идеальной жидкости",
  none,
  (
    (name: "Кортенко А. М.", group: "ФН11-11М"),
    (name: "Куприн А. Д.", group: "ФН11-11М"),
    (name: "Бочкова Т. А.", group: "ФН11-11М")
  ),
  (name: "Герцик Ю. Г."),
)

#outline()
#pagebreak()

= Задание

Определить линию тока для течения вокруг цилиндрического тела диаметром $40 "мм"$, центр которого
расположен посередине между двумя параллельными стенками. Расстояние между стенками равно $80 "мм"$.
На достаточном расстоянии от цилиндра однородный поток жидкости имеет скорость $40 "мм/с"$.

= Теоретическая часть

Для безвихревого течения жидкости применяют два вида потенциалов: 
потенциал скорости $phi : bold(v) = nabla phi$ и функцию тока $psi : bold(v) = nabla times (psi bold(e)_z)$.

Уравнение для функции тока $psi$:
$
(partial^2 psi)/(partial x^2) + (partial^2 psi)/(partial y^2) = 0.
$
Компоненты скорости связаны с $psi$ следующим образом:
$
bold(v)^x = (partial psi)/(partial y), quad bold(v)^y = - (partial psi)/(partial x).
$

== Граничные условия

Используются следующие граничные условия:
+ Условие входа: $bold(v)|_(Sigma_1) = bold(v)_e$;
+ Условие непроницаемой стенки (с проскальзыванием):
  $bold(n) dot bold(v) |_(Sigma_2) = bold(v)_s dot bold(n);$
  
В случае использования функции тока $psi$, условие на непроницаемой стенке становится 
условием Дирихле:
$
psi |_(Sigma_2) = "const",
$
#par(first-line-indent: 0pt)[причём, так как $bold(v) = nabla times (psi bold(e)_z)$, функцию тока мы
находим с точностью до константы, поэтому выберем эту константу равной нулю на нижней границе
области. Тогда значение функции тока на какой-нибудь линии тока будет равно потоку между нижней границей и этой линией тока, следовательно, на верхней границе $psi$ равно полному потоку $Q = U_oo dot H$;
из соображений симметрии, на поверхности цилиндра $psi$ равно $Q / 2 = U_oo dot H / 2$.]

= Генерация сетки

Для генерации сетки была выбрана программа gmsh версии 4.15.0.
Получившийся `.geo` файл для генерации сетки:
```
D = 40;
R = D/2;
H = 80;
L = 200;
U_inf = 40;
mesh_size = 10.0;

Point(1) = {-L, 0, 0, mesh_size};
Point(2) = {L, 0, 0, mesh_size};
Point(3) = {L, H, 0, mesh_size};
Point(4) = {-L, H, 0, mesh_size};

Point(5) = {0, H/2, 0, mesh_size / 2.0};
Point(6) = {R, H/2, 0, mesh_size / 2.0};
Point(7) = {0, H/2 + R, 0, mesh_size / 2.0};
Point(8) = {-R, H/2, 0, mesh_size / 2.0};
Point(9) = {0, H/2 - R, 0, mesh_size / 2.0};
Circle(10) = {6, 5, 7};
Circle(11) = {7, 5, 8};
Circle(12) = {8, 5, 9};
Circle(13) = {9, 5, 6};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Line Loop(20) = {1, 2, 3, 4, -10, -11, -12, -13};
Plane Surface(100) = {20};

Physical Line("Inlet") = {4};
Physical Line("Outlet") = {2};
Physical Line("Walls") = {1, 3};
Physical Line("Cylinder") = {10, 11, 12, 13};
Physical Surface("Fluid") = {100};

Mesh 2;
Mesh.Algorithm = 6;
```

Получившаяся сетка:
#figure(
  image("img/mesh.png")
)

= Решение задачи методом конечных элементов

== Переход к слабой формулировке

Введём пробные функции $delta psi$ такие, что на границе с условием Дирихле (стенки), они равны нулю.
Домножим уравнение Лапласа на $delta psi$ и проинтегрируем по объёму
$
limits(integral.double)_V ( psi_(x x) + psi_(y y) ) dot delta psi space d V = 0,
$
#par(first-line-indent: 0pt)[
  используя формулу Остроградского-Гаусса:
]
$
limits(integral)_(Sigma) bold(n) dot nabla psi dot delta psi space d Sigma - limits(integral.double)_V nabla (psi) dot nabla (delta psi) space d V = 0,
$
#par(first-line-indent: 0pt)[
  первый интеграл при интегрировании по стенкам даёт 0, потому что $delta psi = 0$ на стенках;
  при интегрировании по входу/выходу нормальная производная $bold(n) dot nabla psi = 0$, 
  потому что считаем, что условие входа и выхода достаточно далеки от областей высоких градиентов.
]
$
limits(integral.double)_V nabla (psi) dot nabla (delta psi) space d V = 0.
$

== Аппроксимация методом конечных элементов

Разобъём область $V$ на конечные элементы $V_i$, в которых будем аппроксимировать $psi$ и $delta psi$
следующим образом:
$
psi^((i)) = sum_j^N psi_j dot N_j (x, y), \
delta psi^((i)) = sum_j^N delta psi_j dot N_j (x, y)
$
#par(first-line-indent: 0pt)[
  где $psi_j$ -- значения в узлах конечно-элементной сетки, для линейных треугольных элементов
  это значения в вершинах треугольников.
  $delta psi_j$ -- значения вариации в узлах КЭ сетки.
]
Тогда уравнение принимает вид:
$
sum_i sum_j sum_k psi_j delta psi_k limits(integral.double)_(V_i) nabla N_j dot nabla N_k d V = 0,
$
#par(first-line-indent: 0pt)[
  меняя порядок суммирования, вынесем суммирование по $k$ наружу:
]
