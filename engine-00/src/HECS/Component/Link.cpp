#include "Link.h"
#include <iostream>
#include <iomanip>

// PARENTS ==========================================================================================================
size_t ParentCollection::size() const {
    return n_;
}

std::expected<EntityType, ErrorCode> ParentCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool ParentCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> ParentCollection::add_component(Entity e, Entity parent) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        parents_.push_back(parent);
        if (has_component(parent)) depths_.push_back(depths_[indices_[parent]] + 1); // if the parent already has parents the depth should reflect that
        else depths_.push_back(1);
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        parents_[i] = parent;
        if (has_component(parent)) depths_[i] = depths_[indices_[parent]] + 1;
        else depths_[i] = 1;
        ++n_;
    }
    unsorted_components_.emplace(e);
}

std::expected<void, ErrorCode> ParentCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];
    owners_[i] = owners_[end];
    parents_[i] = parents_[end];
    depths_[i] = depths_[end];
    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
    unsorted_components_.erase(e); // prevent bugs if we destroy a component in during frame we created it
}

void ParentCollection::binary_insertion_sort() {
    int sorted = n_ - unsorted_components_.size();
    auto& keys = depths_; // sort by depth
    for (int i = sorted; i < n_; ++i) {
        auto key = keys[i];
        auto owner = owners_[i];
        auto parent = parents_[i];
        auto depth = depths_[i];

        // find the position where the key should be inserted
        int left = 0, right = sorted - 1;
        if (key <= MAX_OPTIMAL_DEPTH) {
            left = key_boundaries_[key - 1];
        }
        else {
            left = key_boundaries_[MAX_OPTIMAL_DEPTH - 1];
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (keys[mid] > key) {
                    right = mid - 1;
                }
                else {
                    left = mid + 1;
                }
            }
        }

        // shift elements to make space for the new key
        for (int j = i - 1; j >= left; --j) {
            indices_[owners_[j + 1]] = j;
            owners_[j + 1] = owners_[j];
            parents_[j + 1] = parents_[j];
            depths_[j + 1] = depths_[j];
        }
        indices_[owners_[left]] = i;
        owners_[left] = owner;
        parents_[left] = parent;
        depths_[left] = depth;

        // increment sorted size and key boundaries
        ++sorted;
        if (key <= MAX_OPTIMAL_DEPTH) {
            for (int j = key - 1; j < MAX_OPTIMAL_DEPTH; ++j) {
                ++key_boundaries_[j];
            }
        }
    }
    unsorted_components_.clear();
}

void ParentCollection::print() {
    static const int w = 3;
    std::cout << "Owners:  ";
    for (int i = 0; i < n_; ++i) {
        std::cout << std::setw(w) << owners_[i].id << " ";
    }
    std::cout << std::endl;
    std::cout << "Parents: ";
    for (int i = 0; i < n_; ++i) {
        std::cout << std::setw(w) << parents_[i].id << " ";
    }
    std::cout << std::endl;
    std::cout << "Depths:  ";
    for (int i = 0; i < n_; ++i) {
        std::cout << std::setw(w) << depths_[i] << " ";
    }
    std::cout << std::endl;
}


// CHILDREN =========================================================================================================
size_t ChildListCollection::size() const {
    return owners_.size();
}

std::expected<EntityType, ErrorCode> ChildListCollection::index(Entity e) const {
    auto r = indices_.find(e);
    if (r == indices_.end()) return std::unexpected(false);
    else return r->second;
}

bool ChildListCollection::has_component(Entity e) const {
    return indices_.contains(e);
}

std::expected<void, ErrorCode> ChildListCollection::add_component(Entity e, std::unordered_set<Entity> children) {
    if (!indices_.emplace(e, n_).second) return std::unexpected(false);
    if (owners_.size() == n_) {
        owners_.push_back(e);
        child_lists_.push_back(children);
        ++n_;
    }
    else {
        int i = n_ - 1;
        owners_[i] = e;
        child_lists_[i] = children;
        ++n_;
    }
    
}

std::expected<void, ErrorCode> ChildListCollection::destroy_component(Entity e) {
    if (!has_component(e)) return std::unexpected{ false };
    int i = indices_[e];
    Entity last_entity = owners_[n_ - 1];
    int end = indices_[last_entity];
    owners_[i] = owners_[end];
    child_lists_[i] = child_lists_[end];
    indices_[last_entity] = i;
    indices_.erase(e);
    --n_;
}

// ==================================================================================================================
std::expected<void, ErrorCode> LinkCollection::make_child(Entity e, Entity parent) {
    // add parent to e
    if (e == parent) return std::unexpected{ false }; // entity can't be a child of itself
    if (pc.has_component(e)) return std::unexpected{ false }; // entity is already assigned to a different parent

    pc.add_component(e, parent);
    update_child_depths(e);

    // add e to parent's list of children
    if (cc.has_component(parent)) { // parent already has other children
        auto i = *cc.index(parent);
        cc.child_lists_[i].emplace(e);
    }
    else {
        cc.add_component(parent, { e });
    }
}

void LinkCollection::update_child_depths(Entity e) {
    int depth = 0;
    if (pc.has_component(e)) depth = pc.depths_[pc.indices_[e]];
    if (cc.has_component(e)) {
        for (Entity child : cc.child_lists_[cc.indices_[e]]) {
            pc.depths_[pc.indices_[child]] = depth + 1;
            update_child_depths(child);
        }
    }
}

void LinkCollection::sever(Entity e) {
    if (!pc.has_component(e)) return;
    Entity parent = pc.parents_[pc.indices_[e]];
    // remove parent from e:
    pc.destroy_component(e);
    update_child_depths(e);
    // remove e from parent's list of children:
    int i = cc.indices_[parent];
    if (cc.child_lists_[i].size() == 1) cc.destroy_component(parent);
    else cc.child_lists_[i].erase(e);
}

void LinkCollection::sever_children(Entity e) {
    if (!cc.has_component(e)) return;
    int i = cc.indices_[e];
    std::unordered_set<Entity>& l = cc.child_lists_[i];
    for (auto it = l.begin(); it != l.end(); ++it) {
        pc.destroy_component(*it);
        update_child_depths(*it);
    }
    cc.destroy_component(e);
}

void LinkCollection::print(Entity e) const {
    std::cout << "Entity: " << e.id << std::endl;

    std::cout << "Parent: ";
    if (pc.has_component(e)) std::cout << pc.parents_[*pc.index(e)].id;
    else std::cout << "<no parent>";
    std::cout << std::endl;

    std::cout << "Depth: ";
    if (pc.has_component(e)) std::cout << pc.depths_[*pc.index(e)];
    else std::cout << "<no parent>";
    std::cout << std::endl;

    std::cout << "Children: ";
    if (cc.has_component(e)) {
        std::cout << "{ ";
        for (auto child : cc.child_lists_[*cc.index(e)]) {
            std::cout << child.id << " ";
        }
        std::cout << "}";
    }
    else std::cout << "<no children>";
    std::cout << std::endl;
}
