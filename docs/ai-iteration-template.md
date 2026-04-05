# Iteration Task

## Spec

- Must follow `docs/ai-execution-spec.md`
- If this iteration prompt conflicts with the execution spec, the execution spec wins

## Execution Rules

- This iteration must produce a closed capability unit.
- Must fully complete `P1`.
- Must also touch at least one of `P2` / `P3`.
- Must not work on only a single module.
- Must include code + tests + docs.
- Must preserve clear capability vs gap boundary.
- Must not hide unstable behavior as completed capability.
- If diagnostics exist, keep result / diagnostics consistent.

## Focus

### P1 (Primary)
- [module + capability target]

### P2 / P3 (Secondary, must pick at least one)
- [module + target]

---

## Objectives

- Deliver a **representative capability subset**
- Maintain clear boundary between capability and gap
- Ensure deterministic behavior

---

## Required Outputs

### New Capabilities

- [Module]&#58; description

---

### Known Gaps (New / Updated)

- [Module]&#58; description

---

## Required Code Changes

- src/...
- tests/...
- docs/...

---

## Required Tests

### Capability Tests

- [case1]
- [case2]

### Edge-Case Tests

- [boundary case]

### Gap Tests

- [ambiguous / unsupported case]

---

## Commit Message (Structured)

[GeometrySection] ...
[GeometryHealing] ...
[GeometrySearchPoly] ...
