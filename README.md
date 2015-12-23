# picokernel-MSP430
Extremely light weight kernel that can fit in very low memory MSP430 microcontrollers like MSP430G2553. Maximum 6 tasks are supported for MSP430G2553. There is no feature provided for stack overflow detection so use recursion and number of variables within user task carefully.

Some of the stats of the kernel:
<ul>
<li>User task stack frame size: 64 Bytes. out of which 32 Bytes available to user.</li>
<li>Context switching time:80us @ 1Mhz clock.</li>
<li>Round-robin task scheduler with changeable time slice.</li>
</ul>
