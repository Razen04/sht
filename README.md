# SHT: Syntax Hybrid Transpiler in C++

This is a custom Recursive Descent Parser I wrote in C++ to handle a specific subset of Markdown for [GATEQuest](https://github.com/Razen04/GateQuest).

Basically, I needed something that could handle mixed content like Markdown, LaTeX math equations (like $E=mc^2$), tables, and code blocks—without breaking all in one string as I scraped everything for GateOverflow site so the output was mixed with different grammar. Initially for GATEQuest, I just asked an AI to write the parser to render content cause that wasn't something I wanted to focus at that time but I wanted to see if I could build a proper transpiler engine from scratch(obviously with the help of AI guidance) in C++, something which I can own instead of the JS parser made by the AI.

It’s not perfect, but it works, and handles about 2,608 complex questions from my dataset in under 20ms.

## Why C++?

To be honest, the JS version was working fine and maybe it is objectively better. But I wanted to learn how actual parsers work under the hood.

I built this to handle specific parts only which was required for GATEQuest:

1. Math: Inline $...$ and Block $$...$$ (passed through to KaTeX).
2. Tables: Standard Markdown tables with pipe | delimiters.
3. Code: Inline backticks and fenced code blocks.
4. Standard MD: Bold, Italics, Images, Links.

## Technical Details

It uses a standard compiler architecture:

- Lexer (Tokenizer): Scans the raw string and breaks it into tokens (TEXT, BOLD, PIPE, MATH_BLOCK, etc.).
- Parser: A Recursive Descent Parser that constructs an Abstract Syntax Tree (AST). It handles the nesting logic (e.g., "we are inside a table row, so the next pipe means a new cell").
- Renderer: Walks the AST and generates the final HTML string.

## Benchmark Results

I ran a head-to-head benchmark against my Node.js Regex parser on a dataset of 2,608 questions.

- JavaScript (Regex): ~0.001ms per item.
- C++ (Sht): ~0.007ms per item.

Yeah, the JS engine is technically faster for simple cases because V8 is a beast, but Sht is good too, I guess.

## Building and Running

You need a C++17 compiler.

```Bash
# Compile the project
make

# Run it on a JSON file
bin/renderer input.json output.json
```

## Known Issues / TODOs

1. Rendering has an issue in "`$`", basically $ within single backticks.
2. `<div><p>` tags are in options too (in my dataset), which shouldn't happen as it creates unnecessary space.
3. Will try to compile it in WASM(WebAssembly) for GATEQuest.

> It doesn't support 100% of the CommonMark spec, just the parts I need for my dataset.

Use it if you want, but you're probably better off using a battle-tested library unless you're trying to learn how parsers work like I was.

[LICENSE](LICENSE.md)
