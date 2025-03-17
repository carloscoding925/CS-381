// Preprocessor directive - ensures file is only included once
#pragma once

// Macro for OIS support in raylib
#define RAYLIB_SUPPORT_OIS
// Include the raylib library
#include <raylib-cpp.hpp>

// Include memory header file from the standard library
#include <memory>
// Include vector header file from the standard library
#include <vector>
// Include optional header file from the standard library
#include <optional>

// Defines a set of related functions, classes, structs etc.
namespace cs381 {

	/**
	 * @brief Base class for various components that can be attached to an entity.
	 */
	// Component parent class declaration
	class Component {
		// Declares a pointer to an Entity struct. This references the entity that the component is attached to.
		struct Entity* object;
		/* Using the 'friend' keyword allows the Entity struct to access any potential private or protected members of future
		 classes that inherit from the Component class. */
		friend struct Entity;

	// Methods accessible from outside the component class
	public:
		// Returns a reference to the entity which the component is attached ti
		struct Entity& Object() { return *object; }
		// Returns a reference to the transform component of the entity
		struct TransformComponent& Transform();
		// Bool for determining if the component should be active
		bool enabled = true;

		// Constructor for the Component class - Accepts a reference to an Entity struct
		Component(struct Entity& e) : object(&e) {}
		// Constructor for the Component class - Accepts a reference to an Entity struct and a boolean for determining if the component should be active
		Component(struct Entity& e, bool enabled): object(&e), enabled(enabled) {} // Start
		/* Virtual method meant to be overridden by child classes
		When overwritten in child classes, actions to be performed are defined in here */
		virtual void Tick(float dt) {}
		// Destructor for the component class
		// Currenly does nothing, meant to be overridden by child classes to clean up memory
		virtual ~Component() {} // Cleanup

		// Template function - casts the component to a derived type T and returns a reference to it
		// dynamic_cast casts the component to type T, and return a reference if the cast is successful, otherwise returns a nullptr
		template<std::derived_from<Component> T>
		T& as() { return *dynamic_cast<T*>(this); }
	};

	/**
	 * @brief Component storing the positional data of a Entity
	 */
	// Transform Component struct declaration, inherits from the Component class
	struct TransformComponent: public Component {
		// Inherits the constructors from the Component class
		using Component::Component;
		// Vector3 for storing position
		raylib::Vector3 position = {0, 0, 0};
		// Degree for storing object direction
		raylib::Degree heading = 0;
	};


	/**
	 * @brief Represents an object in the game world.
	 */
	// Entity struct declaration
	struct Entity {
		// Vector of 'unique_ptr' pointers to components, ensures that each component is exclusive to the entity
		std::vector<std::unique_ptr<Component>> components = {};

		// Constructor for the Entity struct - also initializes a TransformComponent
		// Every entity will have a transform component initialized with it
		Entity() { AddComponent<TransformComponent>(); } // Entities have a transform by default!
		// Deletes the copy constructor for the Entity struct so they cannot be copied.
		Entity(const Entity&) = delete;

		// Move constructor for the Entity struct - moves an entities components to another entity
		Entity(Entity&& other) : components(std::move(other.components)) {
			// iterate over all our components
			for(auto& component: components)
				// Update the object pointer to point to the new entity
				component->object = this; // When we are moved make sure the components still point to us!
		}

		// Deletes the copy assignment operator so entities cannot be copied.
		Entity& operator=(const Entity&) = delete;
		// Declares a new move assignment operator for the Entity struct
		Entity& operator=(Entity&& other) {
			// Move the components from the other entity to this entity
			components = std::move(other.components);
			// iterate over all our components
			for(auto& component: components)
				// Update the object pointer to point to the new entity
				component->object = this; // When we are moved make sure the components still point to us!
			// return the new entity
			return *this;
		}

		/**
		 * @brief Performs actions during each game update for the entity and its components.
		 * @param dt The time elapsed since the last update.
		 */
		// Calls the Tick method for each component in the entity
		void Tick(float dt) {
			// Iterate through all the components in the entity
			for(auto& componentPtr: components)
				// Check if the component is enabled
				if(componentPtr->enabled)
					// If the component is enabled, call its Tick method
					componentPtr->Tick(dt);
		}

		/**
		 * @brief Adds a new component of type T to the entity.
		 * @tparam T The type of component to add.
		 * @tparam Ts The types of arguments to pass to the component's constructor.
		 * @param args The arguments to pass to the component's constructor.
		 * @return The index of the added component.
		 */
		// Template declaration, checks to make sure T is derived from the Component class, with an N number of arguments
		template<std::derived_from<Component> T, typename... Ts>
		// Function declaration for adding a component to an entity, returns a size_t and accepts N number of arguments
		size_t AddComponent(Ts... args) {
			// Creates a new unique pointer to a component of type T, and passes the entity and arguments to the constructor
			std::unique_ptr<Component> component = std::make_unique<T>(*this, std::forward<Ts>(args) ...);
			// Adds the component to the entity's components vector
			components.push_back(std::move(component));
			// Returns the index of the added component
			return components.size() - 1;
		}

		// How could we remove a component?

		/**
		 * @brief Retrieves a component of type T from the entity.
		 * @tparam T The type of component to retrieve.
		 * @return An optional reference to the component if found, or std::nullopt if not found.
		 */
		// Template declaration, checks to make sure T is derived from the Component class
		template<std::derived_from<Component> T>
		// Function declaration for retrieving a component from an entity, returns an optional reference to the component
		std::optional<std::reference_wrapper<T>> GetComponent() {
			// Check if the component is a TransformComponent
			if constexpr(std::is_same_v<T, TransformComponent>){ // This extra step here allows us to skip itterator overhead since we know the transform component should be in slot 0
				// Attempt to cast the component to type T
				T* cast = dynamic_cast<T*>(components[0].get());
				// If the cast is successful, return a reference to the component
				if(cast) return *cast;
			}
			// Iterate through all the components in the entity
			for(auto& componentPtr: components){
				// Attempt to cast the component to type T
				T* cast = dynamic_cast<T*>(componentPtr.get());
				// If the cast is successful, return a reference to the component
				if(cast) return *cast;
			}
			// If the component is not found, return a null optional
			return {};
		}

		/**
		 * @brief Retrieves this object's transform...
		 * @return The object's transform component.
		 * @note this function has undefined behavior (probably a crash...) if the transform component is ever removed!
		 */
		// Function declaration for retrieving the transform component of an entity
		TransformComponent& Transform() { return *GetComponent<TransformComponent>(); }
	};

	/**
	 * @brief Syntax sugar function that provides access to the object's transform component
	 * @return The object's transform component.
	 * @note this function has undefined behavior (probably a crash...) if the transform component is ever removed!
	 */
	// Function declaration for retrieving the transform component of a component
	TransformComponent& Component::Transform() { return Object().Transform(); }
}