#include "world_state/world_state.hpp"

#include "core/hash.hpp"

namespace ow {

EntityId WorldState::mint(const std::string& class_tag) {
  EntityId id = next_id_++;
  Entity e;
  e.id = id;
  e.class_tag = class_tag;
  e.owner = kTruthOwner;
  e.source = EntitySource::ground_truth;
  e.updated_at = tick_;
  entities_[id] = e;
  ++version_;
  return id;
}

void WorldState::commit_truth(Tick tick,
                              const std::vector<TruthUpdate>& updates) {
  tick_ = tick;
  for (const TruthUpdate& u : updates) {
    auto it = entities_.find(u.id);
    if (it == entities_.end()) continue;
    Entity& e = it->second;
    e.position = u.position;
    e.velocity = u.velocity;
    e.updated_at = tick;
  }
  ++version_;
}

std::vector<Adjudication> WorldState::adjudicate(std::vector<Proposal> proposals,
                                                float assoc_threshold) {
  std::vector<Adjudication> results;
  results.reserve(proposals.size());
  for (Proposal& p : proposals) {
    BeliefSample s;
    s.tick = tick_;
    s.producer = p.producer;
    s.position = p.position.value_or(Vec3{});
    s.confidence = p.confidence;

    if (!p.position.has_value()) {
      results.push_back({false, 0, "no_position"});
      s.reason = "no_position";
      belief_.push_back(s);
      continue;
    }
    const Vec3 pos = *p.position;
    bool found = false;
    float best = assoc_threshold;
    EntityId best_id = 0;
    for (const auto& [id, e] : entities_) {
      if (p.class_tag.has_value() && e.class_tag != *p.class_tag) continue;
      float d = distance(pos, e.position);
      if (d <= best) {  // strict <= plus ordered ids => ties keep smaller id
        best = d;
        best_id = id;
        found = true;
      }
    }
    if (!found) {
      results.push_back({false, 0, "no_association"});
      s.reason = "no_association";
    } else {
      results.push_back({true, best_id, ""});
      s.entity = best_id;
    }
    belief_.push_back(s);
  }
  return results;
}

std::uint64_t WorldState::state_digest() const {
  Hasher h;
  h.label("WorldState");
  h.add_u64(version_);
  h.add_u64(tick_);
  h.add_u64(entities_.size());
  for (const auto& [id, e] : entities_) {
    h.label("E");
    h.add_u64(id);
    h.add_str(e.class_tag);
    h.add_f32(e.position.x);
    h.add_f32(e.position.y);
    h.add_f32(e.position.z);
    h.add_f32(e.velocity.x);
    h.add_f32(e.velocity.y);
    h.add_f32(e.velocity.z);
    h.add_u64(e.updated_at);
  }
  return h.h;
}

}  // namespace ow
