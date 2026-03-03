// Размеры в мм
D = 40;           // Диаметр цилиндра
R = D/2;          // Радиус
H = 80;           // Высота канала
L = 200;          // Длина области (достаточно далеко от цилиндра)
U_inf = 40;       // Скорость набегающего потока
mesh_size = 10.0;  // Размер сетки около границ

// Точки
Point(1) = {-L, 0, 0, mesh_size};       // Левый нижний
Point(2) = {L, 0, 0, mesh_size};        // Правый нижний
Point(3) = {L, H, 0, mesh_size};        // Правый верхний
Point(4) = {-L, H, 0, mesh_size};       // Левый верхний

// Цилиндр (круг)
Point(5) = {0, H/2, 0, mesh_size / 2.0};      // Центр (меньший размер для точности)
Point(6) = {R, H/2, 0, mesh_size / 2.0};
Point(7) = {0, H/2 + R, 0, mesh_size / 2.0};
Point(8) = {-R, H/2, 0, mesh_size / 2.0};
Point(9) = {0, H/2 - R, 0, mesh_size / 2.0};
Circle(10) = {6, 5, 7};
Circle(11) = {7, 5, 8};
Circle(12) = {8, 5, 9};
Circle(13) = {9, 5, 6};

// Линии канала
Line(1) = {1, 2}; // Низ
Line(2) = {2, 3}; // Выход
Line(3) = {3, 4}; // Верх
Line(4) = {4, 1}; // Вход

// Поверхность с дыркой
Line Loop(20) = {1, 2, 3, 4, -10, -11, -12, -13};
Plane Surface(100) = {20};

// Физические группы (Важно для BC!)
Physical Line("Inlet") = {4};
Physical Line("Outlet") = {2};
Physical Line("Wall_Up") = {1};
Physical Line("Wall_Down") = {3};
Physical Line("Cylinder") = {10, 11, 12, 13};
// Physical Surface("Fluid") = {100};

// Генерация 2D сетки (Треугольники)
Mesh 2;
Mesh.Algorithm = 6; // Frontal-Delaunay
