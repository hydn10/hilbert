# Research roadmap

These are promising follow-up experiments rather than committed release work:

- Smooth the phase-shift estimate and use it to locate minima.
- Decouple the analysis sampling frequency from the simulation frequency. Investigate whether 100 Hz is sufficient.
- Add controlled measurement noise.
- Implement a peak-detection method and compare it with Hilbert-transform analysis.
- Simulate a faulty damper.
- Generate deterministic runs for healthy and faulty dampers, then compare the success rates of the candidate analysis methods.

## Deferred design investigations

### Preserve exact counts through fixed-grid time-window filters

Status: deferred until there is a concrete performance or memory need.

The generic filtered sink factory conservatively changes an exact input count into an upper bound. This is necessary for
an arbitrary predicate, but a half-open time-window filter over the current fixed-step simulation grid has a deterministic
output count: sample `i` is produced at `Float(i) * time_step`.

Do not generalize the sink-factory count model for this case yet. The filtered factory currently receives only an input
count, not the sampling schedule needed to establish the filtered count. Extending that protocol would add coupling and
concept machinery for little present benefit. In the phase-scan least-squares sink, exactness would only avoid one integer
increment per accepted observation. Moreover, an incorrect exact count is a broken correctness promise: the
normal-equations reducer uses it for the constant-basis Gram entry and minimum-sample validation without recounting the
observations. Adaptive time stepping would also make the filtered count unknown in general.

Revisit this if filtered vector sinks measurably over-reserve memory, another downstream sink materially benefits from an
exact output count, or the simulation gains an explicit sampling-schedule abstraction. The preferred direction is a
separate fixed-grid/index-window adapter rather than special treatment in the generic predicate filter. It should derive a
half-open integer index range using the same index-to-time mapping as the engine, filter by index so its behavior matches
the advertised count by construction, preserve exactness only for exact fixed-grid inputs, and otherwise return an upper
bound or an unknown count.
