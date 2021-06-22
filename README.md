# FanucBotGui
GUI for move and controll laser-bot

Technologies:
1. C++ language
2. Qt + opencascade
3. qmake build system

Build:
1. Add CSF_OCCTIncludePath - path to opencascade include files;
   example: /usr/include/opencascade
2. Add CSF_OCCTLibPath - path to opencascade lib files;
   example: /usr/lib
   
Using:</br>
Общие требования к интерфейсу
<ol>
    <li>У интерфейса должно быть предусмотрено три режима:
       <ul>
        <li>редактирование задания;</li>
        <li>обработка детали (лазер на роботе);</li>
        <li>обработка детали (лазер неподвижен);</ul></li>
    <li>По умолчанию запускается режим редактирования задания.</li>
    <li>По окончании обработки или при её принудительной остановке интерфейс переходит в режим редактирования. </li>
    <li>Для управления процессом обработки в интерфейсе должны быть предусмотрены кнопки:</li>
        <ul>
        <li>перехода к режимам обработки («Пуск 1», «Пуск 2»);</li>
        <li>приостановки процесса обработки («Пауза»);</li>
        <li>остановки режима обработки и возврат в режим редактирования («Стоп»).</li></li>
        </ul>
   </ol>

В режиме РЕДАКТИРОВАНИЯ ЗАДАНИЯ в интерфейсе доступны следующие функции:
<ol>
    <li>Отображение модели детали (загружается из файла)</li>
    <li>Отключаемое отображение модели оснастки (файл задается в настройках).</li>
    <li>Отображение оснастки вкл/выкл флагом в настройках.</li>
    <li>Установка взаимного расположения детали и оснастки:
        <ul>
        <li>с клавиатуры, путем установки величины смещения и поворота.</li>
        <li>с помощью «мыши».</li>
        </ul></li>
    <li>Расстановка «точек привязки»: </li>
        <ol>
        <li>У каждой точки привязки предусмотрены отображаемые в отдельном окошке:
            -координаты модели;
            -мировые координаты робота (XYZ). </li>
        <li>Запись текущих координат робота в выделенную точку привязки.</li>
        <li>Сброс мировых координат робота в точке привязки.</li>
        <li>Точки привязки с установленными координатами должны быть помечены при отображении на 3D сцене.</li>
        </ol>
    <li>Расстановка «точек обработки»:</li>
        2.6.1 У каждой точки обработки предусмотрены отображаемые в отдельном окошке:
            -координаты модели;
            -направление луча в виде трёх углов;
            -имя файла для программы управления лазером (MaxiGraf);
            -задержка в точке [мс].
        2.6.2 При создании точки обработки направление определяется нормалью к поверхности модели.
    <li>Опциональная установка одной «точки захвата» с возможностью её удаления.</li>
</ol>

В режимах ОБРАБОТКИ в интерфейсе доступны следующие функции:
    3.1 Отображение моделей детали и оснастки (если включена).
    3.2 Отображение модели инструмента (лазерной головки или захвата) в соответствии с реальным положением.
    3.3 Индикация работы лазера.
    3.4 Возможность настройки ориентации и точки начала луча лазера.
    3.5 Отображение сообщения о завершении процесса обработки.
