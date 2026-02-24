#import "@preview/thmbox:0.1.1": *

// всевозможные окружения
#let thmbox = thmbox.with(
  sans-fonts: "New Computer Modern",
  title-fonts: "New Computer Modern",
)
#let todo(
  fill: yellow.lighten(80%),
  variant: "TODO",
  color: red,
  ..args,
) = thmbox(fill: fill, variant: variant, color: color, ..args)
#let unwanted(..args) = thmbox(
  bar-thickness: 8pt,
  fill: tiling(size: (60pt, 60pt))[
    #place(rect(fill: red.lighten(90%), width: 60pt, height: 60pt))
    #place(line(stroke: black.lighten(70%)+1pt, start: (0%, 0%), end: (100%, 100%)))
    #place(line(stroke: black.lighten(70%)+1pt, start: (0%, 100%), end: (100%, 0%)))
  ],
  variant: [#h(-2.5em) НЕОБЯЗАТЕЛЬНОЕ\ это не нужно в билете, но оставлено здесь для понимания #place(top+right, dx: -1em, block(fill: black.lighten(50%), outset: .25em, radius: 25%, text(baseline: -2pt)[#h(2pt)#emoji.skull #emoji.warning #emoji.fire]))#v(-1em)],
  numbering: none,
  color: red,
  ..args
)


#let dark-red = rgb("#C5283D")
#let mint = rgb("3eb489")
#let aqua = rgb("#3fa1d2")
#let dark-blue = rgb("#11388d")
#let purple = rgb("#66118d")
#let purple2 = rgb("#6d50b4")
#let pink = rgb("#c34dd1")
#let womit = rgb("#264a3e")

#let theorem(..args) = thmbox(
  variant: "Теорема",
  color: dark-red,
  ..args,
)
#let theorem_(..args) = thmbox(
  variant: "Теорема",
  color: dark-red,
  numbering: none,
  ..args,
)
#let THEOREM(..args) = thmbox(
  variant: "Теорема",
  color: dark-red,
  fill: dark-red.lighten(85%),
  ..args,
)
#let THEOREM_(..args) = thmbox(
  variant: "Теорема",
  color: dark-red,
  fill: dark-red.lighten(85%),
  numbering: none,
  ..args,
)

#let proposition(..args) = thmbox(
  variant: "Предложение",
  color: mint,
  ..args,
)
#let proposition_(..args) = thmbox(
  variant: "Предложение",
  color: mint,
  numbering: none,
  ..args,
)
#let PROPOSITION(..args) = thmbox(
  variant: "Предложение",
  color: mint,
  fill: mint.lighten(85%),
  ..args,
)
#let PROPOSITION_(..args) = thmbox(
  variant: "Предложение",
  color: mint,
  fill: mint.lighten(85%),
  numbering: none,
  ..args,
)

#let lemma(..args) = thmbox(
  variant: "Лемма",
  color: green,
  ..args,
)
#let lemma_(..args) = thmbox(
  variant: "Лемма",
  color: green,
  numbering: none,
  ..args,
)
#let LEMMA(..args) = thmbox(
  variant: "Лемма",
  color: green,
  fill: green.lighten(85%),
  ..args,
)
#let LEMMA_(..args) = thmbox(
  variant: "Лемма",
  color: green,
  fill: green.lighten(85%),
  numbering: none,
  ..args,
)

#let corollary(..args) = thmbox(
  variant: "Следствие",
  color: aqua,
  ..args,
)
#let corollary_(..args) = thmbox(
  variant: "Следствие",
  color: aqua,
  numbering: none,
  ..args,
)
#let COROLLARY(..args) = thmbox(
  variant: "Следствие",
  color: aqua,
  fill: aqua.lighten(85%),
  ..args,
)
#let COROLLARY_(..args) = thmbox(
  variant: "Следствие",
  color: aqua,
  fill: aqua.lighten(85%),
  numbering: none,
  ..args,
)

#let definition(..args) = thmbox(
  variant: "Определение",
  color: blue,
  ..args,
)
#let definition_(..args) = thmbox(
  variant: "Определение",
  color: blue,
  numbering: none,
  ..args,
)
#let DEFINITION(..args) = thmbox(
  variant: "Определение",
  color: blue,
  fill: blue.lighten(85%),
  ..args,
)
#let DEFINITION_(..args) = thmbox(
  variant: "Определение",
  color: blue,
  fill: blue.lighten(85%),
  numbering: none,
  ..args,
)

#let example(..args) = thmbox(
  variant: "Пример",
  color: purple2,
  ..args,
)
#let example_(..args) = thmbox(
  variant: "Пример",
  color: purple2,
  numbering: none,
  ..args,
)
#let EXAMPLE(..args) = thmbox(
  variant: "Пример",
  color: purple2,
  fill: purple2.lighten(85%),
  ..args,
)
#let EXAMPLE_(..args) = thmbox(
  variant: "Пример",
  color: purple2,
  fill: purple2.lighten(85%),
  numbering: none,
  ..args,
)

#let remark(..args) = thmbox(
  variant: "Замечание",
  color: dark-blue,
  ..args,
)
#let remark_(..args) = thmbox(
  variant: "Замечание",
  color: dark-blue,
  numbering: none,
  ..args,
)
#let REMARK(..args) = thmbox(
  variant: "Замечание",
  color: dark-blue,
  fill: dark-blue.lighten(85%),
  ..args,
)
#let REMARK_(..args) = thmbox(
  variant: "Замечание",
  color: dark-blue,
  fill: dark-blue.lighten(85%),
  numbering: none,
  ..args,
)

#let note(..args) = thmbox(
  variant: "Заметка",
  color: womit,
  ..args,
)
#let note_(..args) = thmbox(
  variant: "Заметка",
  color: womit,
  numbering: none,
  ..args,
)
#let NOTE(..args) = thmbox(
  variant: "Заметка",
  color: womit,
  fill: womit.lighten(85%),
  ..args,
)
#let NOTE_(..args) = thmbox(
  variant: "Заметка",
  color: womit,
  fill: womit.lighten(85%),
  numbering: none,
  ..args,
)

#let algo(..args) = thmbox(
  variant: [`Алгоритм`],
  color: mint.darken(25%),
  ..args,
)
#let algo_(..args) = thmbox(
  variant: [`Алгоритм`],
  color: mint,
  numbering: none,
  ..args,
)
#let ALGO(..args) = thmbox(
  variant: [`Алгоритм`],
  color: mint.darken(25%),
  fill: mint.darken(25%).lighten(85%),
  ..args,
)
#let ALGO_(..args) = thmbox(
  variant: [`Алгоритм`],
  color: mint,
  numbering: none,
  ..args,
)

#let claim(..args) = thmbox(
  variant: "Утверждение",
  color: orange,
  ..args,
)
#let claim_(..args) = thmbox(
  variant: "Утверждение",
  color: orange,
  numbering: none,
  ..args,
)
#let CLAIM(..args) = thmbox(
  variant: "Утверждение",
  color: orange,
  fill: orange.lighten(85%),
  ..args,
)
#let CLAIM_(..args) = thmbox(
  variant: "Утверждение",
  color: orange,
  fill: orange.lighten(85%),
  numbering: none,
  ..args,
)

#let question(..args) = thmbox(
  variant: "Вопрос",
  color: purple,
  ..args,
)
#let question_(..args) = thmbox(
  variant: "Вопрос",
  color: purple,
  numbering: none,
  ..args,
)
#let QUESTION(..args) = thmbox(
  variant: "Вопрос",
  color: purple,
  fill: purple.lighten(85%),
  ..args,
)
#let QUESTION_(..args) = thmbox(
  variant: "Вопрос",
  color: purple,
  fill: purple.lighten(85%),
  numbering: none,
  ..args,
)

#let axiom(..args) = thmbox(
  variant: "Аксиома",
  color: pink,
  ..args,
)
#let axiom_(..args) = thmbox(
  variant: "Аксиома",
  color: pink,
  numbering: none,
  ..args,
)
#let AXIOM(..args) = thmbox(
  variant: "Аксиома",
  color: pink,
  fill: pink.lighten(85%),
  ..args,
)
#let AXIOM_(..args) = thmbox(
  variant: "Аксиома",
  color: pink,
  fill: pink.lighten(85%),
  numbering: none,
  ..args,
)
#let qed() = {
  h(1fr)
  context {
    set text(size: 1.4 * text.size)
    "∎"
    // box(fill: black, width: 0.8 * text.size, height: 0.8 * text.size, radius: 7%)
  }
}
#let proof(
  body
) = context [
  #[
    #set text(style: "oblique", weight: "bold")
    Доказательство:
  ]
  #body #qed()
]
