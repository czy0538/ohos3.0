# Kernel Overview<a name="EN-US_TOPIC_0000001123863153"></a>

-   [Overview](#section1429342661510)
    -   [CPU Architecture Support](#section48891456112819)
    -   [Working Principles](#section4599142312817)


## Overview<a name="section1429342661510"></a>

The OpenHarmony LiteOS-M kernel is a lightweight operating system \(OS\) kernel designed for the IoT field. It features small size, low power consumption, and high performance. The LiteOS-M kernel has simple code structure, including the minimum function set, kernel abstraction layer, optional components, and project directory.

The OpenHarmony LiteOS-M kernel architecture consists of the  hardware layer and the hardware-irrelevant layers, as shown in  [Figure Kernel architecture](#fig17231457191415).

The hardware layer is classified based on the compilation toolchain and chip architecture, and provides a unified Hardware Abstraction Layer \(HAL\) interface to improve hardware adaptation and facilitate the expansion of various types of &IoT hardware and compilation toolchains. The basic kernel provides basic kernel capabilities. The extended modules provide capabilities of components, such as network and file systems, as well as exception handling and debug tools. The Kernel Abstraction Layer \(**KAL**\) provides unified standard APIs.

**Figure  1**  Kernel architecture<a name="fig17231457191415"></a>  
![](figure/kernel-architecture.png "kernel-architecture")

### CPU Architecture Support<a name="section48891456112819"></a>

The CPU architecture includes two layers: general architecture definition layer and specific architecture definition layer. The former provides interfaces supported and implemented by all architectures. The latter is specific to an architecture. For a new architecture to be added, the general architecture definition layer must be implemented first and the architecture-specific functions can be implemented at the specific architecture definition layer.

**Table  1**  CPU architecture rules

<a name="table577192233214"></a>
<table><thead align="left"><tr id="row977192213323"><th class="cellrowborder" valign="top" width="21.592159215921594%" id="mcps1.2.4.1.1"><p id="p15771422123213"><a name="p15771422123213"></a><a name="p15771422123213"></a>Rule</p>
</th>
<th class="cellrowborder" valign="top" width="34.713471347134714%" id="mcps1.2.4.1.2"><p id="p57782211323"><a name="p57782211323"></a><a name="p57782211323"></a>General Architecture Definition Layer</p>
</th>
<th class="cellrowborder" valign="top" width="43.694369436943695%" id="mcps1.2.4.1.3"><p id="p977132223217"><a name="p977132223217"></a><a name="p977132223217"></a>Specific Architecture Definition Layer</p>
</th>
</tr>
</thead>
<tbody><tr id="row1577522103217"><td class="cellrowborder" valign="top" width="21.592159215921594%" headers="mcps1.2.4.1.1 "><p id="p378152243213"><a name="p378152243213"></a><a name="p378152243213"></a>Header file location</p>
</td>
<td class="cellrowborder" valign="top" width="34.713471347134714%" headers="mcps1.2.4.1.2 "><p id="p37819227320"><a name="p37819227320"></a><a name="p37819227320"></a>kernel/arch/include</p>
</td>
<td class="cellrowborder" valign="top" width="43.694369436943695%" headers="mcps1.2.4.1.3 "><p id="p9783225321"><a name="p9783225321"></a><a name="p9783225321"></a>kernel/arch/&lt;arch&gt;/&lt;arch&gt;/&lt;toolchain&gt;/</p>
</td>
</tr>
<tr id="row16781422193218"><td class="cellrowborder" valign="top" width="21.592159215921594%" headers="mcps1.2.4.1.1 "><p id="p978112283215"><a name="p978112283215"></a><a name="p978112283215"></a>Header file name</p>
</td>
<td class="cellrowborder" valign="top" width="34.713471347134714%" headers="mcps1.2.4.1.2 "><p id="p2781322193210"><a name="p2781322193210"></a><a name="p2781322193210"></a>los_&lt;function&gt;.h</p>
</td>
<td class="cellrowborder" valign="top" width="43.694369436943695%" headers="mcps1.2.4.1.3 "><p id="p912752623513"><a name="p912752623513"></a><a name="p912752623513"></a>los_arch_&lt;function&gt;.h</p>
</td>
</tr>
<tr id="row207882213215"><td class="cellrowborder" valign="top" width="21.592159215921594%" headers="mcps1.2.4.1.1 "><p id="p10789229321"><a name="p10789229321"></a><a name="p10789229321"></a>Function name</p>
</td>
<td class="cellrowborder" valign="top" width="34.713471347134714%" headers="mcps1.2.4.1.2 "><p id="p1778202218329"><a name="p1778202218329"></a><a name="p1778202218329"></a>Halxxxx</p>
</td>
<td class="cellrowborder" valign="top" width="43.694369436943695%" headers="mcps1.2.4.1.3 "><p id="p207817224327"><a name="p207817224327"></a><a name="p207817224327"></a>Halxxxx</p>
</td>
</tr>
</tbody>
</table>

LiteOS-M supports mainstream architectures, such as ARM Cortex-M3, ARM Cortex-M4, ARM Cortex-M7, ARM Cortex-M33, and RISC-V. If you need to expand the CPU architecture, see  [Chip Architecture Adaptation](../porting/porting-chip-kernel-overview.md#section137431650339).

### Working Principles<a name="section4599142312817"></a>

Configure the system clock and number of ticks per second in the  **target\_config.h**  file of the development board. Configure the task, memory, inter-process communication \(IPC\), and exception handling modules based on service requirements. When the system boots, the modules are initialized based on the configuration. The kernel startup process includes peripheral initialization, system clock configuration, kernel initialization, and OS boot. For details, see  [Figure 2](#fig19742101817344).

**Figure  2**  Kernel startup process<a name="fig19742101817344"></a>  
![](figure/kernel-startup-process.png "kernel-startup-process")

