# CS315 - Profiler

## Project Documentation: 
- **[Project - Profiler](https://github.com/DigiPen-CS315/DigiPen-CS315/blob/main/projects/project-profiler-instrumented/README.md)**

### Implementation Details 
 Describe your implementation 
- The key idea is to periodically suspend the main thread, retrieve its instruction pointer (RIP), and resolve that address to a function name using SymFromAddr.

- Sampling is performed on a background thread (std::thread)

- The main thread is suspended using SuspendThread, and its context is fetched using GetThreadContext

- At the end of execution, statistics are written to a CSV report file

### Integration Details
 Describe how to integrate your profiler into a project 
 - To integrate the profiler into an existing project, simply include the Profiler class and call its lifecycle functions during program initialization and shutdown.
   
### Requirements  
What if any code/project changes are required to add it to an existing project 
- Include Profiler.h and link Profiler.cpp
- It is made with singleton pattern so can easilly get with getinstance() function.

### Output  
- What is logged <!-- replace this text -->
    - Note the type of output file, naming convention, location, etc. <!-- replace this text -->

| Function                                       | HitCount | Percentage | Time(ms) |
|-----------------------------------------------|----------|------------|----------|
| NtUserMsgWaitForMultipleObjectsEx             | 694      | 79.9539%   | 51.4419  |
| b2ParticleSystem::FindContacts_Reference       | 19       | 2.1889%    | 1.2188   |
| NtGdiDdDDIPresent                              | 18       | 2.0737%    | 0.8846   |
| NtUserPeekMessage                              | 10       | 1.1521%    | 0.8895   |
| NtUserWindowFromDC                             | 9        | 1.0369%    | 0.4855   |
| NtGdiDdDDICreateAllocation                     | 8        | 0.9217%    | 0.3892   |
| NtUserGetDC                                    | 7        | 0.8065%    | 0.3001   |
| b2ParticleSystem::SolveDamping                 | 6        | 0.6912%    | 0.5243   |


- ProfileReport.csv
- This file is created in the root directory(inside RelWithDebInfo) of the project when the program exits.
- If you open *.exe file and press quit button, "ProfileReport.csv" will created.

    

## Continuous Integration Tests  
Edit this document and replace the "liquidfun" with the name of your repository, i.e. `student-project-profiler-liquidfun-ryancdavison`, to display the results of your own tests (not of my template). There are two instances to replace in each link. These badges are only updated from main branch, so you will not see the update until your development branch is merged into `main`.  <!-- you can delete this text -->  
[![CMake_Windows_MSVC](https://github.com/DigiPen-CS315/template-project-profiler-liquidfun-Minki-Cho/workflows/CMake_Windows_MSVC/badge.svg)](https://github.com/DigiPen-CS315/student-project-profiler-Minki-Cho/actions/workflows/WindowsBuild.yml)  
[![CMake_Linux_Clang](https://github.com/DigiPen-CS315/template-project-profiler-liquidfun-Minki-Cho/workflows/CMake_Linux_Clang/badge.svg)](https://github.com/DigiPen-CS315/student-project-profiler-Minki-Cho/actions/workflows/LinuxBuild.yml)  


</br>  

---

<img src="liquidfun/Box2D/Documentation/Programmers-Guide/html/liquidfun-logo-square-small.png"
alt="LiquidFun logo" style="float:right;" />

LiquidFun Version [1.1.0][]

# Welcome to LiquidFun!

LiquidFun is a 2D physics engine for games.  Go to our
[landing page][] to browse our documentation and see some examples.

LiquidFun is an extension of [Box2D][]. It adds a particle based fluid and soft
body simulation to the rigid body functionality of [Box2D][]. LiquidFun can be
built for many different systems, including Android, iOS, Windows, OS X, Linux,
and JavaScript. Please see `Box2D/Documentation/Building/` for details.

Discuss LiquidFun with other developers and users on the
[LiquidFun Google Group][]. File issues on the [LiquidFun Issues Tracker][]
or post your questions to [stackoverflow.com][] with a mention of
**liquidfun**.

Please see [Box2D/Documentation/Building/][] to learn how to build LiquidFun and
run the testbed.

LiquidFun has a logo that you can use, in your splash screens or documentation,
for example. Please see the [Programmer's Guide][] for the graphics and further
details.

For applications on Google Play that integrate this tool, usage is tracked.
This tracking is done automatically using the embedded version string
(`b2_liquidFunVersionString`), and helps us continue to optimize it. Aside from
consuming a few extra bytes in your application binary, it shouldn't affect
your application at all. We use this information to let us know if LiquidFun
is useful and if we should continue to invest in it. Since this is open
source, you are free to remove the version string but we would appreciate if
you would leave it in.

  [LiquidFun Google Group]: https://groups.google.com/forum/#!forum/liquidfun
  [LiquidFun Issues Tracker]: http://github.com/google/liquidfun/issues
  [stackoverflow.com]: http://www.stackoverflow.com
  [landing page]: http://google.github.io/liquidfun
  [1.1.0]: http://google.github.io/liquidfun/ReleaseNotes.html
  [Box2D]: http://box2d.org
  [Box2D/Documentation/Building/]: http://google.github.io/liquidfun/Building.html
  [Programmer's Guide]: http://google.github.io/liquidfun/Programmers-Guide.html
