# CS315 - Memory Debugger

## Project Documentation

This solution will be shared for both Project 4 and 5.
- **[Project - Windows/MSVC Memory Debugger](https://github.com/DigiPen-CS315/DigiPen-CS315/blob/main/projects/project-mem-debugger-windows/README.md)**
- **[Project - Linux/Clang Memory Debugger](https://github.com/DigiPen-CS315/DigiPen-CS315/blob/main/projects/project-mem-debugger-linux/README.md)**

### Compilers  
- Cover which compilers your application targets
    - [x] gcc [version]  
    - [x] clang [version]  
    - [x] msvc/Visual Studio [version]  

### Integration 

#### How to Integrate the Memory Debugger
1. **Include the Header**:  
   Simply add `#include "MemDebugger.h"` in my project’s source files.

2. **Add the Source File**:  
   Include the source file into build. This ensures the debugger’s code is compiled.

3. **Link Required Libraries (Windows-specific)**:  
   For Windows builds, ensure that your project links against `dbghelp.lib`.

4. **Automatic Initialization**:  
   The debugger uses a singleton pattern combined with a nifty-counter for automatic initialization and cleanup. No extra initialization code is required in your project.

#### When a memory leak is detected, the debugger writes detailed information to a CSV log file.
Message,File,Line,Bytes,Address,Additional Infomation

#### Design Decisions in the Memory Debugger

- no-man’s land is created by reserving extra memory when allocating user data. Specifically, during an allocation, it reserve numberOfPages + 1 pages.

- This design ensures that both underflow and overflow errors are more readily detected during debugging.

- `0xfdfdfdfd` ensures that any corruption of this pattern indicates a memory boundary violation, thereby aiding in the rapid identification of memory corruption issues.

- used `std::vector` to store allocation records. A vector allows quick access to any element, which simplifies logging and validation of allocations.

- I replace the standard memory allocation functions by overriding the global `new` operators.

- Global Operator Overriding, Allocation Tracking with a Dynamic Container, Logging

- The entire project took approximately 10hour to complete.
- Research and finding information
- Implementation of the memory tracking and logging features

### Requirements  
- What, if any, code/project changes are required to add it to an existing project 
1. **Include the Header**:  
   Simply add `#include "MemDebugger.h"` in my project’s source files.

2. **Add the Source File**:  
   Include the source file into build. This ensures the debugger’s code is compiled.

3. **Link Required Libraries (Windows-specific)**:  
   For Windows builds, ensure that your project links against `dbghelp.lib`.

4. **Automatic Initialization**:  
   The debugger uses a singleton pattern combined with a nifty-counter for automatic initialization and cleanup. No extra initialization code is required in your project.

### Output  
- What is logged in the event of a memory error or leak? 

        Message,File,Line,Bytes,Address,Additional Infomation

        

 - Describe the type of output file, naming convention, etc. 
    
        ..\build\src\ProjectScenarios\Debug_log.csv

## Continuous Integration Tests  
Edit this document and replace the "template-project-mem-debugger" with the name of your repository, i.e. `student-project-mem-debugger-ryancdavison`, to display the results of your own tests (not of my template). 
- There are two instances to replace in each link. 
- These badges are only updated from `main` branch, so you will not see the update until your development branch is merged into `main`. 
[![CMake_Windows_MSVC](https://github.com/DigiPen-CS315/student-project-mem-debugger-Minki-Cho/workflows/CMake_Windows/badge.svg)](https://github.com/DigiPen-CS315/student-project-mem-debugger-Minki-Cho/actions/workflows/WindowsBuild.yml)  
[![CMake_Linux_Clang](https://github.com/DigiPen-CS315/student-project-mem-debugger-Minki-Cho/workflows/CMake_Linux/badge.svg?)](https://github.com/DigiPen-CS315/student-project-mem-debugger-Minki-Cho/actions/workflows/LinuxBuild.yml)



