# What is this?

What you see is a repository of my solution for task no. 4. of the [Operating Systems (pol. Systemy Operacyjne, SO)](https://usosweb.mimuw.edu.pl/kontroler.php?_action=katalog2/przedmioty/pokazPrzedmiot&prz_kod=1000-214bSOB) course offered by the Faculty of Mathematics, Informatics and Mechanics at the University of Warsaw (further referred to as "MIM UW") in the 2021/2022 summer semester.

# Brief description

The task was to alter the default scheduling strategy for userspace processes in the the operating system [Minix](https://github.com/Stichting-MINIX-Research-Foundation/minix/tree/R3.3.0). The new strategy assigned each process a "bucket" (that one could change with a new system call, which worked in a similar fashion to [`nice`](https://linux.die.net/man/1/nice)), which then the scheduler scanned circularly, always picking the first process of a non-empty bucket if one existed. 

# Full description 

You can find the original task description in Polish [**here**](https://github.com/kfernandez31/SO-2-Processor-Emulator/blob/main/task_description.md).

---
Copyright of the task's description and resources: MIM UW.