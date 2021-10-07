# Dynamic Loading and Linking<a name="EN-US_TOPIC_0000001078523712"></a>

-   [Basic Concepts](#section208951139453)
-   [Working Principles](#section14140155320511)
-   [Development Guidelines](#section133501496612)
    -   [Available APIs](#section874113201669)
    -   [How to Develop](#section196712561563)


## Basic Concepts<a name="section208951139453"></a>

The OpenHarmony dynamic loading and linking mechanism includes a kernel loader and a dynamic linker. The kernel loader loads application programs and the dynamic linker. The dynamic linker loads the shared library on which the application programs depend, and performs symbol relocation for the application programs and shared libraries. Compared with static linking, dynamic linking is a mechanism for delaying the linking of applications and dynamic libraries to run time.

**Advantages of Dynamic Linking**

1.  Dynamic linking allows multiple applications to share code. The minimum loading unit is page. It saves disk and memory space than static linking.
2.  When a shared library is upgraded, the new shared library overwrites the earlier version \(the APIs of the shared library are downward compatible\). You do not need to re-link the shared library.
3.  The loading address can be randomized to prevent attacks and ensure security.

## Working Principles<a name="section14140155320511"></a>

**Figure  1**  Dynamic loading process<a name="fig1797764116422"></a>  
![](figure/dynamic-loading-process.png "dynamic-loading-process")

1.  The kernel maps the  **PT\_LOAD**  section in the ELF file of the application to the process space. For files of the ET\_EXEC type, fixed address mapping is performed based on  **p\_vaddr**  in the  **PT\_LOAD**  section. For files of the ET\_DYN type \(position-independent executable programs, obtained through the compile option  **-fPIE**\), the kernel selects the  **base**  address via  **mmap**  for mapping \(load\_addr = base + p\_vaddr\).
2.  If the application is statically linked \(static linking does not support the compile option  **-fPIE**\), after the stack information is set, the system redirects to the address specified by  **e\_entry**  in the ELF file of the application and runs the application. If the program is dynamically linked, the application ELF file contains the  **PT\_INTERP**  section, which stores the dynamic linker path information \(ET\_DYN type\). The dynamic linker of musl is a part of the  **libc-musl.so**. The entry of  **libc-musl.so**  is the entry of the dynamic linker. The kernel selects the  **base**  address for mapping via the  **mmap**  API, sets the stack information, redirects to the  **base + e\_entry**  \(entry of the dynamic linker\) address, and runs the dynamic linker.
3.  The dynamic linker bootstraps and searches for all shared libraries on which the application depends, relocates the imported symbols, and finally redirects to the  **e\_entry**  \(or  **base + e\_entry**\) of the application to run the application.

**Figure  2**  Program execution process<a name="fig17879151310447"></a>  
![](figure/program-execution-process.png "program-execution-process")

1.  The loader and linker call  **mmap**  to map the  **PT\_LOAD**  section.
2.  The kernel calls  **map\_pages**  to search for and map the existing page cache.
3.  If the memory does not have the required code or data when the program is executed, a page missing interrupt is triggered to read the content of the ELF file into the memory and add the memory block to the page cache.
4.  Map the memory blocks of the file read to the virtual address region.
5.  The program continues to run.

The program is executed with continuous page missing interrupts.

## Development Guidelines<a name="section133501496612"></a>

### Available APIs<a name="section874113201669"></a>

LOS\_DoExecveFile

**Function prototype:**

INT32 LOS\_DoExecveFile\(const CHAR \*fileName, CHAR \* const \*argv, CHAR \* const \*envp\);

**Function**: Executes a new user program based on  **fileName**.

**Parameter Description**

<a name="table13709103919318"></a>
<table><thead align="left"><tr id="row1170923910316"><th class="cellrowborder" valign="top" width="11.92%" id="mcps1.1.3.1.1"><p id="p1709123911313"><a name="p1709123911313"></a><a name="p1709123911313"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="88.08%" id="mcps1.1.3.1.2"><p id="p1970910395313"><a name="p1970910395313"></a><a name="p1970910395313"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row7709113923117"><td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.1.3.1.1 "><p id="p1870983993114"><a name="p1870983993114"></a><a name="p1870983993114"></a>fileName</p>
</td>
<td class="cellrowborder" valign="top" width="88.08%" headers="mcps1.1.3.1.2 "><p id="p870963913111"><a name="p870963913111"></a><a name="p870963913111"></a>Name of a binary executable file. It can be a path name.</p>
</td>
</tr>
<tr id="row0709163973120"><td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.1.3.1.1 "><p id="p170993918319"><a name="p170993918319"></a><a name="p170993918319"></a>argv</p>
</td>
<td class="cellrowborder" valign="top" width="88.08%" headers="mcps1.1.3.1.2 "><p id="p9294182194420"><a name="p9294182194420"></a><a name="p9294182194420"></a>Parameters, ended with <strong id="b159331722646"><a name="b159331722646"></a><a name="b159331722646"></a>NULL</strong>, required for program execution. If no parameter is required, set this parameter to <strong id="b4955529344"><a name="b4955529344"></a><a name="b4955529344"></a>NULL</strong>.</p>
</td>
</tr>
<tr id="row341018142206"><td class="cellrowborder" valign="top" width="11.92%" headers="mcps1.1.3.1.1 "><p id="p6411121420204"><a name="p6411121420204"></a><a name="p6411121420204"></a>envp</p>
</td>
<td class="cellrowborder" valign="top" width="88.08%" headers="mcps1.1.3.1.2 "><p id="p4411161412011"><a name="p4411161412011"></a><a name="p4411161412011"></a>New environment variables, ended with <strong id="b64091250517"><a name="b64091250517"></a><a name="b64091250517"></a>NULL</strong>, required for program execution. If no new environment variable is required, set this parameter to <strong id="b12210561156"><a name="b12210561156"></a><a name="b12210561156"></a>NULL</strong>.</p>
</td>
</tr>
</tbody>
</table>

### How to Develop<a name="section196712561563"></a>

The kernel cannot directly call the  **LOS\_DoExecveFile**  API to start a new process. This API is generally called through the  **execve**  API to create a new process using the system call mechanism.

