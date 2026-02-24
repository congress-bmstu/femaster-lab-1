#import "@preview/codly:1.3.0": *
#import "@preview/codly-languages:0.1.10": *

#import "utils-environments.typ" : *

#let notes(
  title: "",
  author: "",
  doc,
) = {
  // установка метаданных документа
  set document(
    title: title,
    author: author,
  )
  // параметры страницы (поля, размер, нумерация страниц)
  set page(
    paper: "a4",
    margin: (
      left: 10mm,
      right: 10mm,
      top: 10mm,
      bottom: 10mm,
    ),
    numbering: "1",
  )
  // шрифт
  set text(
    font: "New Computer Modern",
    size: 14pt,
    lang: "ru",
    region: "RU",
    // weight: "thin",
  )
  // абзацы
  set par(
    first-line-indent: (amount: 1.25cm, all: true), // абзацный отступ
    leading: 0.75em, // межстрочный интервал (1.5em/2)
    justify: true, // выравнивание по ширине
    linebreaks: "optimized",
    spacing: 0.75em, // интервал между параграфами
  )
  // нумерация разделов
  set heading(
    numbering: "1.1",
    outlined: true,
    supplement: [Раздел], // слово используемое при ссылке
    hanging-indent: -12.5mm,
  )
  // колхозный фикс (добавление отсупа разделам и первому абзацу внутри них)
  show heading: it => {
    let sizes = (
      "1": 24pt,
      "2": 16pt,
      "3": 14pt,
    )
    let alignments = (
      "1": left,
      "2": left,
      "3": left,
    )
    // let content = if it.level == 1 { it.body } else { it }
    let content = it
    pad(left: 1.25cm, align(alignments.at(str(it.level)), text(size: sizes.at(str(it.level)), weight: "bold", content)))
    par(spacing: 0.75em)[#text(
      size: 0mm,
    )[#h(0.0em)]] // когда https://github.com/typst/typst/pull/5768 войдет в релиз, убрать это
  }
  // обнуление счетчиком рисунков таблиц и уравнений в каждой главе
  show heading.where(level: 2): it => {
    counter(math.equation).update(0)
    counter(figure.where(kind: image)).update(0)
    counter(figure.where(kind: table)).update(0)
    counter(figure.where(kind: raw)).update(0)
    it
  }
  // нумерация уравнений
  // set math.equation(numbering: "(1)")
  // формат лейблов у уравнений
  set math.equation(numbering: num => {
    let header_nums = counter(heading).get()
    let chap = str(header_nums.at(0))
    let sec = str(header_nums.at(1, default: ""))
    "(" + (chap + "." + sec + "." + str(num)) + ")"
  })
  // автоматическое добавление номера уравнениям при ссылке на них
  // show math.equation: it => {
  //   if it.block and not it.has("label") [
  //     #counter(math.equation).update(v => v - 1)
  //     #math.equation(it.body, block: true, numbering: none)
  //   ] else {
  //     it
  //   }
  // }
  // нумерация и подпись к рисункам
  set figure(
    // placement: auto,
    supplement: [Рисунок], // слово используемое при ссылке
    numbering: num => str(counter(heading).get().at(0)) + "." + str(num), // нумерация внутри раздела
  )
  // Убирает точку. Было "Рисунок 1.1. Подпись" Стало: "Рисунок 1.1 Подпись"
  set figure.caption(separator: " ")
  show figure.caption: it => {
    set par(leading: 0.5em)
    it
  }
  // Подпись к таблицам
  show figure.where(kind: table): set figure(supplement: [Таблица])
  // Возможность figure пересекать границу страницы и быть перенесенной на следующую (для длинных теорем)
  show figure: set block(breakable: true)

  // оглавление
  set outline(
    title: none,
    indent: auto, // автоматические табуляции
    depth: 2,
  )
  set outline.entry(
    fill: repeat([.], gap: 5pt), // многоточие после пунктов
  )
  // подпись СОДЕРЖАНИЕ посередине
  show outline: it => {
    text(
      weight: "bold",
      [
        #align(center, text(size: 20pt)[СОДЕРЖАНИЕ])
        #align(right, [c.])
      ],
    )
    it
  }
  // для заголовков первого уровня жирный шрифт
  show outline.entry.where(level: 1): it => {
    // set text(font: heading-font, size: info-size)
    v(1em, weak: false)
    link(
      it.element.location(),
      text(
        weight: "bold",
        size: 16pt,
        {
          h(-1.25cm)
          it.body() + " "
          text(weight: "light", box(width: 1fr, repeat([.], gap: 5pt)))
          " " + it.page()
        },
      ),
    )
  }

  // пункты в ненумерованных списоках
  set list(
    tight: false,
    // marker: ([---], [---], [---]),
    spacing: 1em,
    indent: 0.55cm,
  )
  set enum(indent: 0.55cm)
  show list: it => {
    // v(0.5em)
    it
    v(0.5em)
  }
  // если пункт списка на несколько строк, то он будет весь выровнен по абзацному отступу, это фикс
  // теперь первая строка с отсупом, остальное нормально (ну собственно так и должно быть)
  // show list.item: it => {
  //   par(hanging-indent: -1.25cm, it)
  // }

  // библиография
  set bibliography(
    style: "gost-r-705-2008-numeric",
    title: "СПИСОК ИСПОЛЬЗОВАННЫХ ИСТОЧНИКОВ",
  )
  // чтобы ссылки отображались как (число)
  show ref: it => {
    let eq = math.equation
    let el = it.element
    if el != none and el.func() == eq {
      // Override equation references.
      link(
        el.location(),
        numbering(
          el.numbering,
          ..counter(eq).at(el.location()),
        ),
      )
    } else {
      // Other references as usual.
      it
    }
  }

  ////////// НАЧАЛО ДОКУМЕНТА //////////
  show: thmbox-init() // инициализация плагина с окружениями
  show: codly-init.with() // инициализация плагина с красивыми блоками кода
  codly(languages: codly-languages)
  doc
}
// центрированная глава
#let centerchapter(text) = {
  context counter(heading).update(counter(heading).get().at(0) + 1)
  align(center)[#heading(numbering: none, text)]
}
// возможность вставлять пдфку
#import "@preview/muchpdf:0.1.0": muchpdf
#let includepdf(pdf_file, ..args) = {
  muchpdf(read(pdf_file, encoding: none), ..args)
}


