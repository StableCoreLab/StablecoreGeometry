# Geometry Capability Matrix

Last Updated: 2026-04-05

---

## Iteration Discipline

- Every iteration must deliver a closed capability unit.
- Every iteration must fully complete `P1` and touch at least one of `P2` / `P3`.
- Capability / edge-case / gap tests are all mandatory at iteration level.
- Capability means deterministic, reproducible, test-covered behavior.
- Unsupported or ambiguous behavior must remain explicitly listed as gap.

---

## Section

| Capability | Status | Tests | Notes |
|----------|--------|------|------|
| Coplanar face merge | ✅ | ✔ | stable |
| Multi-face coplanar strip | ✅ | ✔ | stable |
| Frame with hole | ✅ | ✔ | stable |
| Non-planar section perimeter (basic) | ✅ | ✔ | deterministic subset |
| Mixed coplanar + non-planar coexist | ✅ | ✔ | representative subset |
| Mixed area + open curve adjacency | ⚠️ partial | ✔ | arbitration incomplete |
| Non-planar dominant stitching | ⚠️ partial | ✖ | limited |
| Cross-fragment merge (convex gap) | ❌ | ✖ | unsupported |
| Non-manifold contour stitching | ❌ | ✖ | unsupported |

---

## Healing

| Capability | Status | Tests | Notes |
|----------|--------|------|------|
| Conservative trim-backfill | ✅ | ✔ | stable |
| Single-shell boundary cap | ✅ | ✔ | stable |
| Multi-shell independent cap | ✅ | ✔ | deterministic subset |
| Mixed body shared-edge shell cap | ✅ | ✔ | representative subset |
| Multi-shell shared-edge arbitration | ⚠️ partial | ✔ | limited conflict handling |
| Non-planar shell repair | ❌ | ✖ | unsupported |
| Mesh-body joint healing | ❌ | ✖ | unsupported |

---

## SearchPoly

| Capability | Status | Tests | Notes |
|----------|--------|------|------|
| Candidate ranking | ✅ | ✔ | stable |
| Smallest-containing candidate | ✅ | ✔ | stable |
| Diagnostics consistency | ✅ | ✔ | stable |
| Candidate-level penalty explanation | ✅ | ✔ | basic |
| Synthetic edge classification | ⚠️ partial | ✔ | limited |
| Fake-edge causal explanation | ⚠️ partial | ✖ | shallow |
| Ambiguous recovery | ❌ | ✖ | unsupported |

---

## BodyBoolean

| Capability | Status | Tests | Notes |
|----------|--------|------|------|
| Identical body union/intersection | ✅ | ✔ | stable |
| Disjoint bodies | ✅ | ✔ | stable |
| Axis-aligned box overlap | ✅ | ✔ | subset |
| Face-touching union | ✅ | ✔ | subset |
| Face-touching external difference | ✅ | ✔ | subset |
| Touching empty intersection | ✅ | ✔ | subset |
| Non-axis-aligned overlap | ❌ | ✖ | unsupported |
| Rich touching intersection | ❌ | ✖ | unsupported |
