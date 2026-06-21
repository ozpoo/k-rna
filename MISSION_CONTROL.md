# MISSION_CONTROL.md
# Kärna + .kn — Living Design Document
# Started: 2026
# Status: Seedling

---

## The Vision

A unified computing stack built for humans.
Not for enterprises. Not for committees. For anyone who wants to
understand, build, and own their tools.

One kernel. One language. One rendering model.
All the way from hardware to pixels.

---

## The Problem

Modern computing is a tower of abstractions:

```
hardware
    → firmware
        → OS
            → runtime
                → framework
                    → language
                        → GUI toolkit
                            → finally: your idea
```

Each layer adds mystery. Each layer adds gatekeeping.
Most people never get to the bottom. Most people never feel ownership.

We collapse the tower.

---

## The Stack

```
┌─────────────────────────────────┐
│         your .kn program        │  ← what you write
├─────────────────────────────────┤
│         .kn language            │  ← one language, all the way down
├─────────────────────────────────┤
│         semantic renderer       │  ← GUI is just .kn, no toolkit
├─────────────────────────────────┤
│         Kärna kernel            │  ← bare metal, you can read it
├─────────────────────────────────┤
│         hardware                │  ← x86 now, ARM next
└─────────────────────────────────┘
```

No mystery layers. No hidden abstractions.
Anyone can read any part of this stack.

---

## Kärna (kernel)

**Status:** v0.6 — working, multitasking, paging enabled

**Repository:** mykernel/
**Language:** C + x86 assembly
**Runs on:** QEMU (x86), real hardware next

**What it does today:**
- Boots via Multiboot
- VGA text driver
- Interrupt handling (IDT, PIC)
- Keyboard input
- Physical memory manager (bitmap allocator)
- Virtual memory (x86 two-level page tables, paging enabled)
- Cooperative scheduler (pure asm context switch)
- Centralized config (config.h)

**Roadmap:**
- v0.7: Multiboot info parsing — real RAM detection
- v0.8: VBE framebuffer — real resolution, pixel renderer
- v0.9: Preemptive scheduler
- v1.0: Syscall interface
- v1.1: ELF loader
- v1.2: Ramdisk + VFS
- v1.3: ARM port (Samsung Galaxy Tab)
- v2.0: RISC-V (fully open ISA, no boot blobs)

**Philosophy:**
- Every line has a reason
- Portable by design (arch/ layer)
- Readable by anyone

---

## .kn (language)

**Status:** Design phase — no implementation yet

**File extension:** .kn
**Substrate:** Rust + LLVM (planned)
**Targets:** Kärna kernel, x86, ARM, eventually RISC-V

### Core Philosophy

> Everything is a pattern that can be named, composed, and rendered.

One abstraction. All the way down.
A button is a pattern. A page is a pattern.
A network request is a pattern. A pixel is a pattern.

### Type System

One type: `val`
Everything is a value.
No type tower. No casting. No surprises.

```kn
val name = "Kärna"
val count = 42
val flag = true
```

### Keywords

Plain English. No symbols where words work.
Readable by someone who has never programmed.

```kn
define greet with name:
    show "hello " + name

when button pressed:
    greet with "world"
```

### Memory

Dual model — automatic by default, manual when you want it.
You choose your level of control.

```kn
val x = 42           // automatic
own val y = 42       // manual, you manage it
```

### Cross-environment

Same .kn file can target:
- Kärna kernel (bare metal)
- x86 desktop
- ARM mobile
- RISC-V embedded

The runtime layer handles the difference.
You write once.

---

## Semantic GUI

**Status:** Concept phase

### The Idea

HTML's genius was declarative intent — you say what
a thing *is*, not how to draw it. We take that further.

The GUI is not a separate framework.
The GUI is just .kn.
Kärna renders it natively. No browser. No DOM. No JS engine.

### Syntax sketch

```kn
page "main" {
    header {
        text "Kärna" size:large color:white
    }

    body {
        button "click me" -> greet()
        input "type here" -> on_type()
        list items -> render_item()
    }

    footer {
        text "built with .kn"
    }
}
```

### Rendering model

Every element is a pattern.
Patterns compose.
The renderer walks the pattern tree and draws pixels.
No mystery. No framework. Just patterns → pixels.

### Layout engine

Inspired by HTML/CSS flow but simpler.
One layout model. No quirks mode.
No box model surprises.

```kn
layout:stack     // vertical
layout:row       // horizontal  
layout:grid n    // n columns
layout:free      // absolute positioning
```

---

## Values

These aren't marketing. They're design constraints.

**Agency** — you own your tools. You can read every layer.
**Opportunity** — accessible to anyone. No gatekeeping.
**Growth** — the codebase teaches as you read it.
**Fairness** — open source, forever.
**Efficiency** — no fat. Every abstraction earns its place.
**Reusability** — portable, composable, hackable.

---

## Influences + Prior Art

| Project     | What we learned                              |
|-------------|----------------------------------------------|
| Oberon      | OS + language + GUI as one unified thing     |
| Smalltalk   | Everything is live and inspectable           |
| Collapse OS | Minimal, rebuilable from scratch             |
| HTML        | Declarative intent over imperative drawing   |
| Plan 9      | Everything is a file, clean namespace        |
| Redox OS    | Rust-based microkernel, right philosophy     |
| Asahi Linux | Heroic open hardware reverse engineering     |

None got accessibility right. That's the gap we fill.

---

## Open Questions

- What is the minimal set of patterns needed for a useful GUI?
- How does .kn handle concurrency? (Kärna tasks map to what?)
- What does a .kn package/module system look like?
- How do we make the renderer fast enough without a GPU driver?
- What does debugging look like when you own the whole stack?

---

## Session Log

### Session 1 (2026)
- Built Kärna v0.6 from scratch
- Boot stub, VGA, IDT, PIC, keyboard, PMM, VMM, scheduler
- Cooperative multitasking working
- config.h centralized
- Named the project: Kärna
- Conceived .kn language + semantic GUI
- This document created

### Next session
- Kärna v0.7: multiboot info parsing
- Begin .kn language spec sketch
- Define the single abstraction more precisely

---

## Name

**Kärna** — Swedish for kernel/core.
The seed. The center. Everything grows from here.

**.kn** — the extension. Short. Unmistakable. Yours.