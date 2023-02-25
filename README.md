# Programming_Project

This is the final project for the course in "Programming in C". This course is a first year course in the degree of Tecnological Physics Engeneering in Instituto Superior Técnico - Universidade de Lisboa.

The goal of this project is to create a simulation using C and the visual interface GTK3 of the following physical system with one mass and two springs:

<img width="526" alt="system" src="https://user-images.githubusercontent.com/31959975/221367061-83cd95d0-8192-4731-9cf7-a02d689ba4f8.png">

In this system, the block has mass $M$, the constant of both springs is $K$ and the pendulum has length $l$ and mass $m$. The mass m can only move horizontaly and friction will be ignored. The following initial conditions must be chosen by the user: masses - $M$ and $m$, the springs constant - $K$, the pendulum length - $l$, the initial pendulum angle - $\theta$, the initial position of mass $M$ - $x$, the initial angular velocity of the pendulum - $\omega$ and the initial velocity of mass $M$ - $v$.

The user can visualize real time plots of the mass M position, $x$, and / or the angle of the pendulum, $\theta$ has a function of time. Another possibility is the plot of the the evolution of $x$ with $\theta$.

The simulation window has the following aspect:

<img width="1236" alt="window" src="https://user-images.githubusercontent.com/31959975/221367678-38fa220f-a1ff-47e6-afba-06fbf20195f3.png">

There are default values defined for all parameters but the user can use the minus or plus button to change any of them. Then, the user can start and pause the simulation anytime or even reset the parameters.

