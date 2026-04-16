# ABS Slip Control using System Identification, Advanced Control Methods: Model Predictive Control, Pole Placement, LQR .

## Overview
This project presents the modeling, identification, and control of an Anti-lock Braking System (ABS) in MATLAB/Simulink.

The main objective was to regulate wheel slip in the optimal adhesion region during braking, preventing wheel lock while preserving vehicle stability and steering capability. The project follows a complete control-engineering workflow, from simulation-based data acquisition and system identification to state-space controller design and closed-loop validation.

## Control Objective
The target was to maintain the slip ratio in the optimal range of **0.6–0.8**, using a reference value of **0.7**, while keeping the braking command within the physical constraint **u ∈ [0, 1]**.

## Main Contributions
- Acquired braking and slip data from the **INTECO ABS simulation environment**
- Built a **second-order discrete state-space model** from simulation data
- Evaluated multiple identification methods and selected **N4SID**, refined with **PEM**
- Implemented and compared three control strategies:
  - **Pole Placement**
  - **LQR**
  - **Model Predictive Control (MPC)**
- Designed controllers for slip tracking around the optimal adhesion point
- Applied physical actuator limits to the brake command
- Validated the final solution in both **MATLAB** and **Simulink**

## Workflow
1. **Data acquisition** from the INTECO ABS model in Simulink  
2. **System identification** using input-output slip data  
3. **State-space control design** with Pole Placement, LQR, and MPC  
4. **Closed-loop simulation** in MATLAB  
5. **Validation in Simulink** using the full ABS model  
6. **Performance comparison** between controlled and uncontrolled responses  

## System Identification
The controller design starts from ABS simulation data exported from Simulink:
- **Input:** braking signal
- **Output:** measured wheel slip

Several identification approaches were explored, including classical parametric models. The final model was obtained using **N4SID**, then refined with **PEM**, resulting in a **second-order discrete state-space model**.

This choice was important because the state-space formulation provides the matrices **A, B, C, D** directly, making the model immediately suitable for:
- state-feedback design,
- LQR,
- MPC.

The identified model achieved a reported fit of **83.9%**, which was sufficient for controller design and closed-loop validation.

## Control Strategies

### 1. Pole Placement
Pole Placement was used as a first state-feedback approach on the identified ABS model.

It was mainly useful for:
- verifying controllability,
- shaping the closed-loop dynamics through explicit pole selection,
- understanding the trade-off between fast response and aggressive control action.

Pole placement was tested on both the **N4SID model** and the **N4SID + PEM refined model**, providing a solid baseline for comparison with the more advanced controllers.

### 2. LQR Control
An **LQR controller** was implemented on the discrete state-space model identified with N4SID.

#### LQR design
The controller was built by:
- extracting the matrices `A`, `B`, `C`, `D` from the identified model,
- defining the weighting matrices:
  - `Q = 10 * (C' * C)` to penalize slip deviation,
  - `R = 0.2` to penalize control effort,
- computing the optimal feedback gain,
- adding a **reference precompensator**  for tracking the desired slip reference.

This controller introduced an optimal-control perspective into the project by balancing tracking performance and control effort in a systematic way.

### 3. Model Predictive Control (MPC)
The most advanced and most thoroughly developed controller in the project is **MPC**.

#### MPC design
The final MPC controller was configured with:
- **Slip reference:** `0.7`
- **Prediction horizon:** `25`
- **Control horizon:** `2`
- **Brake constraints:** `[0, 1]`
- **Output weight:** prioritizes slip tracking
- **Input-rate weight:** penalizes abrupt brake variations

## Why MPC Was Chosen?
Although Pole Placement and LQR were both relevant and useful, **MPC was chosen as the final control strategy** because it offered the best overall balance between:
- tracking accuracy,
- smooth control action,
- predict future system behavior,
- optimize the control action online,
- explicitly include actuator constraints,
- reduce aggressive brake variations,
- provide smoother and more realistic braking behavior.

For an ABS application, this is especially important because the braking command must remain physically feasible while the slip ratio must stay close to the optimal adhesion region.

## Results
The project showed strong closed-loop performance in simulation:

- the LQR controller provided effective reference tracking on the identified model
- the MPC controller achieved the best overall behavior in the final validation stage
- slip tracking around the target value **0.7** was achieved without wheel lock
- MATLAB tests showed fast tracking with **no overshoot**
- Simulink validation confirmed stable braking behavior under model constraints
- the reported settling time in Simulink was approximately **0.875 s**

Overall, the controlled system performed significantly better than the uncontrolled case, where slip tends toward full wheel lock.

## Engineering Concepts
This project demonstrates a complete practical workflow in control engineering:
- data-driven modeling,
- state-space identification,
- controller comparison,
- optimal control design,
- predictive control with constraints,
- closed-loop validation in MATLAB/Simulink.

It also highlights the transition from classical state-feedback design to modern constrained predictive control in a realistic ABS application.

## Tools and Technologies
- MATLAB
- Simulink
- System Identification Toolbox
- Control System Toolbox
- Model Predictive Control Toolbox
- INTECO ABS Laboratory System

## Future Improvements
- improve robustness against mismatch between the identified model and the full nonlinear ABS plant
- extend the approach to more realistic nonlinear tire-road interaction models
- further refine MPC tuning for improved performance in the full Simulink environment
