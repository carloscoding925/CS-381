// ECS hpp file declaration
#ifndef ECS_HPP
#define ECS_HPP

// File includes
#include <memory>
#include <concepts>
#include <vector>
#include <deque>
#include <iostream>
#include <ranges>
#include <bitset>
#include <span>
#include <variant>
#include <cassert>

// Component counter to be used when assigning IDs to components
extern size_t globalComponentCounter;
// Namespace declaration, used when grouping related code
namespace cs381 {
	// template declaration so the GetComponentID function can accept any kind of component
	template<typename T>
	// Returns the ID of the component type passed to it
	size_t GetComponentID(T reference = {}) {
		// Declares a new id variable and assigns it the value of the global component counter
		static size_t id = globalComponentCounter++;
		// Returns the id
		return id;
	}

	// Gives an alias to the Entity type, which is a uint8_t
	using Entity = uint8_t; 

	// Defines a struct for the ComponentStorage, which is used to store component data
	struct ComponentStorage {
		// The size of the element in bytes
		size_t elementSize = -1;
		// A vector of bytes to store the component data
		std::vector<std::byte> data;

		// Constructor for the ComponentStorage struct
		ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}
		// Constructor for the ComponentStorage struct that takes the size of the element and reserves space for the data
		ComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }
		
		// Constructor for the ComponentStorage struct that takes a reference to a component, which is used to determine the size of the element
		template<typename Tcomponent>
		ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

		// Function to get a component of type Tcomponent from the storage
		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {
			// Ensures that the component we're trying to get is the same size as the element size (which it should be)
			assert(sizeof(Tcomponent) == elementSize);
			// Ensures that the entity index is valid and within the bounds of the data vector
			assert(e < (data.size() / elementSize));
			// We now look for this component in the data vector and return it
			return *(Tcomponent*)(data.data() + e * elementSize);
		}

		// This function is responsible for allocating memory for a component of type Tcomponent
		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate(size_t count = 1) {
			// Ensures that the component we're trying to allocate is the same size as the element size (which it should be)
			assert(sizeof(Tcomponent) == elementSize);
			// Checks that the number of component we're allocating is less than 100
			assert(count < 100);
			// Stores the original size of the data vector
			auto originalEnd = data.size();
			// Resizes the data vector to accommodate the new component
			data.insert(data.end(), elementSize * count, std::byte{0});
			// We now iterate through all the components to construct them
			for(size_t i = 0; i < count - 1; i++) // Skip the last one
				// Here, we construct the new component in the data vector
				new(data.data() + originalEnd + i * elementSize) Tcomponent();
			return {
				// Here, we contruct the last component in the vector and return it along with the index
				*new(data.data() + data.size() - elementSize) Tcomponent(),
				data.size() / elementSize
			};
		}

		// This function is responsible for making sure a component type T exists already, if not, we allocate space for it
		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			// Check that the component we're accessing exists
			assert(sizeof(Tcomponent) == elementSize);
			// Calculates the current number of components in the data vector
			size_t size = data.size() / elementSize;
			// Here, we check if we have enough space to allocate memory for the component
			if (size <= e)
				// Here, we do the memory allocation for the component
				Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size + 1, 1));
			// Returns a reference to the component type T
			return Get<Tcomponent>(e);
		}
	};

	// Use a template declaration to define a Scene struct that can use any kind of component storage
	template<typename Storage = ComponentStorage>
	struct Scene {
		// Each mask is a vector of booleans, which indicate if a component is present for a given entity
		std::vector<std::vector<bool>> entityMasks;
		// Each storage is a vector of Storage objects, which are used to store the component data
		std::vector<Storage> storages = {Storage()};

		// This function is responsible for getting the storage for a specific component type
		template<typename Tcomponent>
		Storage& GetStorage() {
			// Get the ID of the component type
			size_t id = GetComponentID<Tcomponent>();
			// Since the component id corresponds to how many components we have, we need to make sure we have enough space in the storage vector
			// If we don't have enough space, we need to allocate more
			if(storages.size() <= id)
				// Here, we insert new Storage objects into the vector until we have enough space
				storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), Storage());
			// Here, we check if the element size is max, which indicates that we need to create a new Storage object
			if (storages[id].elementSize == std::numeric_limits<size_t>::max())
				// If the element size is max, we need to create a new Storage object
				storages[id] = Storage(Tcomponent{});
			// Finally, we return the storage for the component type
			return storages[id];
		}

		// This function is responsible for creating a new entity
		Entity CreateEntity() {
			// Here, we create a new entity by adding a new mask to the entityMasks vector
			Entity e = entityMasks.size();
			// By using emplace_back, we allocate memory for the new mask
			entityMasks.emplace_back(std::vector<bool>{false});
			// returns the new entity
			return e;
		}

		// This function adds a component to an entity
		template<typename Tcomponent>
		Tcomponent& AddComponent(Entity e) {
			// Get the ID of the component type (how many components we have)
			size_t id = GetComponentID<Tcomponent>();
			// Here we retrieve the mask for the entity e
			auto& eMask = entityMasks[e];
			// We check if the mask is large enough to accommodate the new component
			if(eMask.size() <= id)
				// If not, we resize the mask to accommodate the new component
				eMask.resize(id + 1, false);
			// We now mark the component as present for the entity
			eMask[id] = true;
			// We can now return the storage for the component type and allocate space for it if needed
			return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);
		}

		// This function removes a component from an entity
		template<typename Tcomponent>
		void RemoveComponent(Entity e) {
			// Get the ID of the component type
			size_t id = GetComponentID<Tcomponent>();
			// Here we retrieve the mask for the entity e
			auto& eMask = entityMasks[e];
			// Here we check if the mask has an entry for the component type
			if(eMask.size() > id)
				// If it does, we remove the component from the mask by marking it as 'absent' or false
				eMask[id] = false;
		}

		// This function gets a component from an entity
		template<typename Tcomponent>
		Tcomponent& GetComponent(Entity e) {
			// Get the ID of the component type
			size_t id = GetComponentID<Tcomponent>();
			// Here we retrieve the mask for the entity e and make sure the component is present
			assert(entityMasks[e][id]);
			// We now return the storage for the component type and get the component from it
			return GetStorage<Tcomponent>().template Get<Tcomponent>(e);
		}

		// This function checks if the entity has a specified component
		template<typename Tcomponent>
		bool HasComponent(Entity e) {
			// Grab the components ID
			size_t id = GetComponentID<Tcomponent>();
			// If the id is a valid entry within the mask and it is marked as present, we return true
			return entityMasks[e].size() > id && entityMasks[e][id];
		}
	};




	// Niceties!

	struct SkiplistComponentStorage {
		// The size of the element in bytes
		size_t elementSize = -1;
		// A vector of size_t's to store the indecies of the components
		std::vector<size_t> indecies;
		// A vector of bytes to store the component data
		std::vector<std::byte> data;

		// Constructor for the SkiplistComponentStorage struct
		SkiplistComponentStorage() : elementSize(-1), indecies(1, -1), data(1, std::byte{0}) {}
		// Constructor for the SkiplistComponentStorage struct that takes the size of the element and reserves space for the data
		SkiplistComponentStorage(size_t elementSize) : elementSize(elementSize) { data.reserve(5 * elementSize); }
		
		// Constructor for the SkiplistComponentStorage struct that takes a reference to a component, which is used to determine the size of the element
		template<typename Tcomponent>
		SkiplistComponentStorage(Tcomponent reference = {}) : SkiplistComponentStorage(sizeof(Tcomponent)) {}

		// Function that retrives a reference to a component of type Tcomponent from the storage
		template<typename Tcomponent>
		Tcomponent& Get(Entity e) {
			// Ensures that the component we're trying to get is the same size as the element size (which it should be)
			assert(sizeof(Tcomponent) == elementSize);
			// Ensures that the entity index is valid and within the bounds of the indecies vector
			assert(e < indecies.size());
			// Ensure the indecies vector has a valid entry for the entity
			assert(indecies[e] != std::numeric_limits<size_t>::max());
			// We now look for this component in the data vector and return it
			return *(Tcomponent*)(data.data() + indecies[e]);
		}

		template<typename Tcomponent>
		std::pair<Tcomponent&, size_t> Allocate() {
			// Ensures that the component we're trying to allocate is the same size as the element size (which it should be)
			assert(sizeof(Tcomponent) == elementSize);
			// Expands the data vector to accommodate the new component
			data.insert(data.end(), elementSize, std::byte{0});
			// Returns a pair containing a reference to the new component and its index
			return {
				// Here, we contruct the new component in the data vector
				*new(data.data() + data.size() - elementSize) Tcomponent(),
				// The index of the new component is the size of the data vector divided by the element size
				(data.size() - 1) / elementSize
			};
		}

		// This function allocates memory for a component of type Tcomponent
		template<typename Tcomponent>
		Tcomponent& Allocate(Entity e) {
			// Calls the Allocate function to allocate memory for the component
			auto [ret, i] = Allocate<Tcomponent>();
			// Stores the byte offset of the component in the indecies vector
			indecies[e] = i * elementSize;
			// Returns a reference to the component
			return ret;
		}

		// This function is responsible for making sure a component type T exists already, if not, we allocate space for it
		template<typename Tcomponent>
		Tcomponent& GetOrAllocate(Entity e) {
			// Check that the component we're accessing exists
			assert(sizeof(Tcomponent) == elementSize);
			// Calculates the current number of components in the data vector
			if (indecies.size() <= e)
				// If the vector is too small, we need to resize it
				indecies.insert(indecies.end(), std::max<int64_t>(int64_t(e) - indecies.size() + 1, 1), -1);
			// Here we check if the entity e has a valid entry in the indecies vector
			if (indecies[e] == std::numeric_limits<size_t>::max())
				return Allocate<Tcomponent>(e);
			// Return the component from the storage
			return Get<Tcomponent>(e);
		}
	};

	// Gives an alias to post_increment_t type as an integer
	using post_increment_t = int;

	// This struct is used to create a view of the scene, which allows us to iterate over entities that have specific components
	template<typename... Tcomponents>
	struct SceneView {
		// The scene that we are viewing
		Scene<SkiplistComponentStorage>& scene;

		// This struct is used to create a sentinel value for the end of the view, used to indicate the end of the iteration
		struct Sentinel {};
		// This struct is used to create an iterator for the view, which allows us to iterate over the entities in the scene
		struct Iterator {
			// The scene that we are viewing
			Scene<SkiplistComponentStorage>* scene = nullptr;
			// The current entity that we are iterating over
			Entity e;

			// A method to check if the scene has the required components
			bool valid() { return (scene->HasComponent<Tcomponents>(e) && ...); }

			// Checks to see if the iterator has reached the end of the scene
			bool operator==(Sentinel) { return scene == nullptr || e >= scene->entityMasks.size(); }

			// Increments the entity index and checks if the iterator is still valid
			Iterator& operator++(post_increment_t) { 
				// Increment entity index
				do {
					e++;
					// Check if we are still within the bounds of the scene
				} while(!valid() && e < scene->entityMasks.size());
				// Returns a reference to the updated iterator
				return *this;
			}
			// Implements the post increment operator
			Iterator operator++() {
				// Creates a copy of the current iterator
				Iterator old;
				// Advances the iterator
				operator++(0);
				// Returns the original iterator
				return old;
			}

			// This function is used to dereference the iterator and get the components of the entity
			// Entity operator*() { return e; }
			std::tuple<std::add_lvalue_reference_t<Tcomponents>...> operator*() { return { scene->GetComponent<Tcomponents>(e)... }; }
		};

		// Returns an iterator to the beginning of the scene
		Iterator begin() { 
			// Create an iterator and set the entity index to 0
			Iterator out{&scene, 0}; 
			// Check if the iterator is valid, if not, increment it until it is valid
			if(!out.valid()) ++out;
			// Return the iterator
			return out;
		}
		// Returns a sentinel value to indicate the end of the scene
		Sentinel end() { return {}; }
	};
}

#endif // ECS_HPP