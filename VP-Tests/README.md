# Software Requirement Specification (SRS)

This repository contains the Program Design document for an uni software engineering project.

## Overview

This project documents the program design derived from a prototype requirements. The document was created by a student team through collaborative interviews and analysis to establish a common understanding of the assigned sub-system.

## Features

- Clean, professional Program Design document structure
- Modular chapter organization (introduction, description, requirements, etc.)
- Cover page with team information

## Structure

<!-- STRUCTURE:START -->
```text
.
├── appendix
│   └── AnhangPD.pdf
├── chapter
│   ├── appendix.tex
│   ├── description.tex
│   └── introduction.tex
├── diagrams
│   ├── ClassAndSubsystems.png
│   ├── ClassDiagramEmergencyStop.pdf
│   ├── ComponentOveriew.png
│   ├── ComponentOverview.pdf
│   ├── E.png
│   ├── H.png
│   ├── HardwareMapping.png
│   ├── K.png
│   ├── LoggingParameters.png
│   ├── N.png
│   ├── S.png
│   ├── SequenceChangingState.png
│   ├── SequenceDriveControlUnit.png
│   ├── SequenceError.png
│   ├── SequenceHuman.png
│   ├── SequenceLogging.png
│   ├── SequenceNormalOp.png
│   ├── SequenceShutdown.png
│   ├── SequenceStartup.png
│   └── SequenceTTC-Observer.png
├── notes
│   └── notes.tex
├── pages
│   ├── cover.tex
│   └── revision.tex
├── Scripts
│   └── update-structure.ps1
├── .gitignore
├── LICENSE
├── pd.tex
└── README.md
```
<!-- STRUCTURE:END -->

This section is generated automatically and reflects the current folder layout of the repository. You can update it by running `update-structure.ps1` in a PowerShell terminal:

```bash
.\Scripts\update-structure.ps1
```

## Building the Document

To compile the LaTeX document and generate the PDF:

```bash
pdflatex srs.tex
```

Or use `latexmk` for automated compilation:

```bash
latexmk -pdf srs.tex
```


## Project Goals

- **Goal**: Derive a programm design based on predefined requirements by the customer
- **Approach**: Following teaching instructions and hoping
- **Deliverable**: Comprehensive programm design document

## Requirements

- LaTeX distribution (TeX Live, MiKTeX, or MacTeX)
- A LaTeX editor (e.g., TeXstudio, Overleaf, VS Code with LaTeX Workshop)

## Template Reference

This project uses the [SRS-Tex](https://github.com/jpeisenbarth/SRS-Tex) template structure by Jean-Philippe Eisenbarth as some orientation.

## License

See [LICENSE](LICENSE) file for details.



