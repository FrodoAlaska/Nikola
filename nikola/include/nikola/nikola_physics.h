#pragma once

#include "nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Physics ***

///---------------------------------------------------------------------------------------------------------------------
/// Consts 

/// The default maximum number of bodies a world can have. 
///
/// @NOTE: This value will be set as a default in the `PhysicsWorldDesc` 
/// upon the creation of the physics world.
const u32 PHYSICS_DEFAULT_MAX_BODIES              = 2048;

/// The default maximum number of body pairs to process at a time. 
///
/// @NOTE: This value will be set as a default in the `PhysicsWorldDesc` 
/// upon the creation of the physics world.
const u32 PHYSICS_DEFAULT_MAX_BODY_PAIRS          = 10240;

/// The default maximum number of constraints a world can have. 
///
/// @NOTE: This value will be set as a default in the `PhysicsWorldDesc` 
/// upon the creation of the physics world.
const u32 PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS = 10240;

/// A value to indicate an invalid ID for any physics object.
const u32 PHYSICS_ID_INVALID = ((u32)-1);

/// Consts 
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBroadPhaseLayer
enum PhysicsBroadPhaseLayer {
  /// The broad phase layer where all static bodies will live.
  PHYSICS_BROAD_PHASE_LAYER_STATIC  = 0,
  
  /// The broad phase layer where all dynamic bodies will live.
  PHYSICS_BROAD_PHASE_LAYER_DYNAMIC,

  /// The maximum number of broad phase layers that 
  /// currently can be used.
  PHYSICS_BROAD_PHASE_LAYERS_MAX,
};
/// PhysicsBroadPhaseLayer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsObjectLayer
enum PhysicsObjectLayer {
  /// This flag indicates a "none" layer, 
  /// which can be used to disable collisions 
  /// with certain layers in the layers matrix.
  PHYSICS_OBJECT_LAYER_NONE = 0,
  
  /// Physics layers for objects that collides with both 
  /// the STATIC and DYNAMIC broad phase layers.
  
  PHYSICS_OBJECT_LAYER_0,
  PHYSICS_OBJECT_LAYER_1,
  PHYSICS_OBJECT_LAYER_2,
  PHYSICS_OBJECT_LAYER_3,
  PHYSICS_OBJECT_LAYER_4,
  PHYSICS_OBJECT_LAYER_5,

  /// Physics layers for objects that collides with  
  /// the _only_ STATIC broad phase layer.
  
  PHYSICS_OBJECT_LAYER_6,
  PHYSICS_OBJECT_LAYER_7,
  
  /// Physics layers for objects that collides with  
  /// the _only_ DYNAMIC broad phase layer.
  
  PHYSICS_OBJECT_LAYER_8,
  PHYSICS_OBJECT_LAYER_9,

  /// The maximum number of layers that currently 
  /// can be used.
  PHYSICS_OBJECT_LAYERS_MAX,
};
/// PhysicsObjectLayer
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyType
enum PhysicsBodyType {
  /// Indicates to a physics body to be static, 
  /// completely unmoving, even by external forces.
  PHYSICS_BODY_STATIC, 

  /// Indicates to a physics body to be dynmaic. 
  /// Affected by all forces. 
  PHYSICS_BODY_DYNAMIC, 

  /// Indicates to a physics body to be kinematic. 
  /// Not affected by external forces, but movable.
  PHYSICS_BODY_KINEMATIC, 
};
/// PhysicsBodyType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// ColliderType
enum ColliderType {
  /// Based to the `collider_create` function 
  /// to construct a box collider.
  COLLIDER_BOX = 0, 
  
  /// Based to the `collider_create` function 
  /// to construct a sphere collider.
  COLLIDER_SPHERE, 
  
  /// Based to the `collider_create` function 
  /// to construct a capsule collider.
  COLLIDER_CAPSULE,
};
/// ColliderType
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// GroundState
enum GroundState {
  /// A value indicating a character is 
  /// currently touching a ground surface.
  GROUND_STATE_ON_GROUND = 0, 

  /// A value indicating a character is 
  /// currently touching a steep surface.
  GROUND_STATE_ON_STEEP_GROUND, 

  /// A value indicating a character is 
  /// on touching a surface but is not 
  /// supported by it and should fall.
  GROUND_STATE_NOT_SUPPORTED, 

  /// A value indicating a character is currently 
  /// not touching anything and is falling.
  GROUND_STATE_IN_AIR,
};
/// GroundState
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBody
struct PhysicsBody;
/// PhysicsBody
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider
struct Collider;
/// Collider
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character
struct Character;
/// Character
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CollisionData
struct CollisionData {
  /// The physics body that were involved in the collision.
  
  PhysicsBody* body1;
  PhysicsBody* body2; 

  /// An offset to which all contacts are relative to.
  Vec3 base_offset      = Vec3(0.0f);
  
  /// The world space normal generated from the collision. 
  Vec3 normal           = Vec3(0.0f);

  /// The penetration depth of the collision,
  /// that is by how much both bodies were touching. 
  f32 penetration_depth = 0.0f;
};
/// CollisionData
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastDesc
struct RayCastDesc {
  /// The starting point of the ray.
  Vec3 origin    = Vec3(0.0f); 

  /// The direction which the ray will be pointing towards.
  Vec3 direction = Vec3(0.0f);

  /// The distance of the ray towards `direction`.
  ///
  /// @NOTE: If you wanted to simulate an "infinite" ray, 
  /// simply make this number large, i.e `100000000.0f`.
  f32 distance   = 1.0f;

  /// The broad phase layer that the ray will be generated in.
  PhysicsBroadPhaseLayer broad_phase_layer = PHYSICS_BROAD_PHASE_LAYER_DYNAMIC;

  /// The ray will only collide with objects in this layer.
  PhysicsObjectLayer object_layer          = PHYSICS_OBJECT_LAYER_0;
};
/// RayCastDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// RayCastResult
struct RayCastResult {
  /// The body that was hit by the ray.
  PhysicsBody* body  = {};

  /// The exact point in world space of the hit point.
  Vec3 point         = Vec3(0.0f);

  /// The original direction of the ray that hit the body. 
  Vec3 ray_direction = Vec3(0.0f);
};
/// RayCastResult
///---------------------------------------------------------------------------------------------------------------------
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsLayersMatrix
using PhysicsLayersMatrix = PhysicsObjectLayer[PHYSICS_OBJECT_LAYERS_MAX][PHYSICS_OBJECT_LAYERS_MAX];
/// PhysicsLayersMatrix
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsWorldDesc
struct PhysicsWorldDesc {
  /// The maximum number of bytes allowed to be allocated 
  /// in the physics world. 
  ///
  /// @NOTE: By default, this value is set to `MiB(10)`.
  sizei allocater_size = MiB(10);

  /// The maximum number of physics jobs allowed in the simulation.
  ///
  /// @NOTE: By default, this value is set to `256`.
  u32 max_jobs     = 256; 

  /// The maximum number of memory barriers allowed in the simulation.
  ///
  /// @NOTE: By default, this value is set to `16`.
  u32 max_barriers = 16;

  /// The maximum number of threads allowed in the simulation.
  ///
  /// @NOTE: By default, this value is set to `-1`, 
  /// which will let the physics world decide how many threads to use.
  i32 max_threads  = -1;
  
  /// The maximum number of body mutexes allowed in the simulation.
  ///
  /// @NOTE: By default, this value is set to `0`, 
  /// which will let the physics world decide how many to use.
  u32 max_mutexes  = 0;
  
  /// The maximum number of bodies allowed in the simulation.
  ///
  /// @NOTE: By default, this value is set to `PHYSICS_DEFAULT_MAX_BODIES`. 
  u32 max_bodies              = PHYSICS_DEFAULT_MAX_BODIES;
  
  /// The maximum number of body pairs to process at a time.
  ///
  /// @NOTE: By default, this value is set to `PHYSICS_DEFAULT_MAX_BODY_PAIRS`. 
  u32 max_body_pairs          = PHYSICS_DEFAULT_MAX_BODY_PAIRS;
  
  /// The maximum number of contact constraints to process at a time.
  ///
  /// @NOTE: By default, this value is set to `PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS`. 
  u32 max_contact_constraints = PHYSICS_DEFAULT_MAX_CONTACT_CONSTRAINTS;

  /// A layer matrix determining the collision relationship each layer has with the each other. 
  ///
  /// @NOTE: By default, all entries in the matrix are set to `PHYSICS_OBJECT_LAYER_NONE`
  PhysicsLayersMatrix layers_matrix; 

  /// The initial gravity of the simulation. 
  /// This value can later be changed using the 
  /// `physics_world_set_gravity` function.
  /// 
  /// @NOTE: By default, this value is set to `Vec3(0.0f, -9.81f, 0.0f)`. 
  Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f);
};
/// PhysicsWorldDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// PhysicsBodyDesc
struct PhysicsBodyDesc {
  /// The starting position of the physics body.
  Vec3 position;
  
  /// The starting rotation of the physics body.
  Quat rotation;
  
  /// The starting body type of the physics body.
  PhysicsBodyType type;
  
  /// The object layer which this body will live in.
  PhysicsObjectLayer layers;

  /// The pre-created collider to assign to this body.
  Collider* collider;

  /// The starting restitution of the physics body.
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `0.1f` by default.
  f32 restitution    = 0.1f;

  /// The starting friction of the physics body.
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `0.0f` by default.
  f32 friction       = 0.0f;

  /// The starting factor which will determine by 
  /// how much the physics body is affected by gravity. 
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `1.0f` by default.
  f32 gravity_factor = 1.0f;

  /// User-specific data to keep in the physics body 
  /// for later retrieval.
  void* user_data  = 0;

  /// If this flag is set to `true`, the body will 
  /// act as a sensor, which will detect collisions 
  /// but never resolve them. Effectively, using it 
  /// as a trigger.
  ///
  /// @NOTE: By default, this value is set to `false`.
  bool is_sensor = false;    
};
/// PhysicsBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CharacterBodyDesc
struct CharacterBodyDesc {
  /// The starting position of the character.
  Vec3 position;
  
  /// The starting rotation of the character.
  Quat rotation;
  
  /// The object layer which the character will live in.
  PhysicsObjectLayer layer; 

  /// Determines the maximum angle (in radians) of 
  /// slope which is allowed to be climbed by the character. 
  /// Any value beyond this member will make the character slide.
  ///
  /// @NOTE: This value is set to `0.872665f` (or `50.0f` in degrees) by default.
  f32 max_slope_angle = (50.0f * DEG2RAD);
  
  /// The mass of the character's body. 
  ///
  /// @NOTE: This value is set to `80.0f` by default.
  f32 mass            = 80.0f;
  
  /// The starting friction of the character body.
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `0.5f` by default.
  f32 friction        = 0.5f;

  /// The starting factor which will determine by 
  /// how much the physics body is affected by gravity. 
  /// Make sure to set this value between a range of `[0, 1]`. 
  ///
  /// @NOTE: This value is set to `1.0f` by default.
  f32 gravity_factor  = 1.0f;

  /// The axis that will represent the character's up direction. 
  /// This will be used to detemine slopes, ground surfaces, and so on.
  ///
  /// @NOTE: This value is set to `Vec3(0.0f, 1.0f, 0.0f)` by default.
  Vec3 up_axis           = Vec3(0.0f, 1.0f, 0.0f);
  
  /// The pre-created collider to assign to this character.
  Collider* collider     = {};
  
  /// User-specific data to keep in the physics body 
  /// for later retrieval.
  void* user_data          = 0;
};
/// CharacterBodyDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// BoxColliderDesc
struct BoxColliderDesc {
  /// The half size of the box collider to 
  /// be created.
  Vec3 half_size; 
};
/// BoxColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// SphereColliderDesc
struct SphereColliderDesc {
  /// The radius of the sphere collider to 
  /// be created.
  f32 radius; 
};
/// SphereColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// CapsuleColliderDesc
struct CapsuleColliderDesc {
  /// The half height of the capsule collider to 
  /// be created.
  f32 half_height;
  
  /// The radius of the capsule collider to 
  /// be created.
  f32 radius; 
};
/// CapsuleColliderDesc
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics world functions

/// Initialize the physics world with the given information in the given `desc`.
NIKOLA_API void physics_world_init(const PhysicsWorldDesc& desc);

/// Shutdown the physics world, reclaiming any allocated memory in the process.
NIKOLA_API void physics_world_shutdown();

/// Step through the physics simulation every frame, using the given `delta_time` and `collision_steps`.
///
/// The `collision_steps` parametar determines the number of iterations to go through a "physics step", 
/// which consists of collision detection followed by numerical integration.
/// The higher the value, the more accurate the physics is, but the more performance will suffer.
///
/// @NOTE: The `collision_steps` parametar is set to `1` by default.
NIKOLA_API void physics_world_step(const f32 delta_time, const i32 collision_steps = 1);

/// Create and allocate a physics body using the information provided in the given `desc`, 
/// returning back a valid `PhysicsBody` to be used later.
NIKOLA_API PhysicsBody* physics_world_create_body(const PhysicsBodyDesc& desc);

/// Add a previously-created physics body `body` to the physics world. The `is_active` parametar
/// indicates whether to wake the body upon addition or not.
NIKOLA_API void physics_world_add_body(const PhysicsBody* body, const bool is_active = true);

/// Add a previously-created character body `character` to the physics world. The `is_active` parametar
/// indicates whether to wake the body upon addition or not.
NIKOLA_API void physics_world_add_character(const Character* character, const bool is_active = true);

/// Combines the `physics_world_create_body` and `physics_world_add_body` functions into one for convenience.
NIKOLA_API PhysicsBody* physics_world_create_and_add_body(const PhysicsBodyDesc& desc, const bool is_active = true);

/// Remove the given `body` from the physics world.
///
/// @NOTE: Make sure to call `physics_world_destroy_body` to remove 
/// the body from the world completely.
NIKOLA_API void physics_world_remove_body(PhysicsBody* body);

/// Destroy the given `body`, deallocating any memory and disabling it in the world. 
///
/// @NOTE: Make sure to call `physics_world_remove_body` _before_ this function.
NIKOLA_API void physics_world_destroy_body(PhysicsBody* body);

/// Combines the `physics_world_destroy_body` and `physics_world_remove_body` functions into one for convenience.
NIKOLA_API void physics_world_destroy_and_remove(PhysicsBody* body);

/// Remove the given `character` from the physics world.
///
/// @NOTE: Make sure to call `character_body_destroy` to remove 
/// the character completely.
NIKOLA_API void physics_world_remove_character(Character* character);

/// Cast a ray using the information provided by `cast_desc` into the world, 
/// firing the `EVENT_PHYSICS_RAYCAST_HIT` event upon any successful intersections.
NIKOLA_API const bool physics_world_cast_ray(const RayCastDesc& cast_desc);

/// When safe mode is on, the physics world will access the bodies in a 
/// multithreaded-fashion, making sure to surround the body access functions 
/// with mutex locks. 
///
/// Otherwise, the physics world will not have this thread-safe accessing.
///
/// @WARNING: You _MUST_ first turn safe mode off when accessing bodies 
/// from events and then turn it back on when you're done. Otherwise, the 
/// physics world will assert and crash the program.
NIKOLA_API void physics_world_set_safe_mode(const bool safe);

/// Set the gravity of the physics world to the given `gravity`.
NIKOLA_API void physics_world_set_gravity(const Vec3& gravity);

/// Set the internal collision relationship between the layers.
NIKOLA_API void physics_world_set_layers_matrix(PhysicsLayersMatrix matrix);

/// Retrieve the current gravity value of the physics world.
NIKOLA_API Vec3 physics_world_get_gravity();

/// Toggle "pause" mode either on or off, depending on the current state. 
/// Pause mode will simply stop updating the physics world, which prompts 
/// all the bodies to stop simulating physics.
NIKOLA_API void physics_world_toggle_paused();

/// Retrieve the current pause state of the physics world.
NIKOLA_API const bool physics_world_is_paused();

/// Physics world functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Physics body functions

/// Set the position of the given `body` to `position`.
/// The `activate` flag indicates whether to wake the body after this operation or not.
NIKOLA_API void physics_body_set_position(PhysicsBody* body, const Vec3 position, const bool activate = true);

/// Set the rotation of the given `body` to `rotation`.
/// The `activate` flag indicates whether to wake the body after this operation or not.
NIKOLA_API void physics_body_set_rotation(PhysicsBody* body, const Quat rotation, const bool activate = true);

/// Set the rotation of the given `body` to using the `axis` and `angle`.
/// The `activate` flag indicates whether to wake the body after this operation or not.
NIKOLA_API void physics_body_set_rotation(PhysicsBody* body, const Vec3 axis, const f32 angle, const bool activate = true);

/// Set the transform of the given `body` to `transform`.
/// The `activate` flag indicates whether to wake the body after this operation or not.
NIKOLA_API void physics_body_set_transform(PhysicsBody* body, const Transform& transform, const bool activate = true);

/// Set the linear velocity of the given `body` to `velocity`.
NIKOLA_API void physics_body_set_linear_velocity(PhysicsBody* body, const Vec3 velocity);

/// Set the angular velocity of the given `body` to `velocity`.
NIKOLA_API void physics_body_set_angular_velocity(PhysicsBody* body, const Vec3 velocity);

/// Set the active state of the given `body` to `active`.
NIKOLA_API void physics_body_set_active(PhysicsBody* body, const bool active);

/// Set the internal user data of the given `body` to `user_data`.
NIKOLA_API void physics_body_set_user_data(PhysicsBody* body, const void* user_data);

/// Set the object layer of the given `body` to `layer`.
NIKOLA_API void physics_body_set_layer(PhysicsBody* body, const PhysicsObjectLayer layer);

/// Set the restitution of the given `body` to `restitution`.
NIKOLA_API void physics_body_set_restitution(PhysicsBody* body, const f32 restitution);

/// Set the friction of the given `body` to `friction`.
NIKOLA_API void physics_body_set_friction(PhysicsBody* body, const f32 friction);

/// Set the gravity factor of the given `body` to `gravity_factor`.
NIKOLA_API void physics_body_set_gravity_factor(PhysicsBody* body, const f32 factor);

/// Set the body type of the given `body` to `type`.
NIKOLA_API void physics_body_set_type(PhysicsBody* body, const PhysicsBodyType type);

/// Set the collider of the given `body` to `collider_id`.
/// The `activate` flag indicates whether to wake the body after this operation or not.
///
/// @NOTE: The given `collider_id` _MUST_ be valid, otherwise the function will assert.
NIKOLA_API void physics_body_set_collider(PhysicsBody* body, const Collider* collider_id, const bool activate = true);

/// Apply linear velocity `velocity` to the given `body`. 
NIKOLA_API void physics_body_apply_linear_velocity(PhysicsBody* body, const Vec3 velocity);

/// Apply force `force` to the given `body`. 
NIKOLA_API void physics_body_apply_force(PhysicsBody* body, const Vec3 force);

/// Apply force `force` at `point` to the given `body`. 
NIKOLA_API void physics_body_apply_force_at(PhysicsBody* body, const Vec3 force, const Vec3 point);

/// Apply torque force `torque` to the given `body`. 
NIKOLA_API void physics_body_apply_torque(PhysicsBody* body, const Vec3 torque);

/// Apply impulse force `impulse` to the given `body`. 
NIKOLA_API void physics_body_apply_impulse(PhysicsBody* body, const Vec3 impulse);

/// Apply impulse force `impulse` at `point` to the given `body`. 
NIKOLA_API void physics_body_apply_impulse_at(PhysicsBody* body, const Vec3 impulse, const Vec3 point);

/// Apply angluar impulse force `impulse` to the given `body`. 
NIKOLA_API void physics_body_apply_angular_impulse(PhysicsBody* body, const Vec3 impulse);

/// Retrieve the position of the given `body`.
NIKOLA_API const Vec3 physics_body_get_position(const PhysicsBody* body);

/// Retrieve the center of mass position of the given `body`.
NIKOLA_API const Vec3 physics_body_get_com_position(const PhysicsBody* body);

/// Retrieve the quaternion rotation of the given `body`.
NIKOLA_API const Quat physics_body_get_rotation(const PhysicsBody* body);

/// Retrieve the linear velocity of the given `body`.
NIKOLA_API const Vec3 physics_body_get_linear_velocity(const PhysicsBody* body);

/// Retrieve the angular velocity of the given `body`.
NIKOLA_API const Vec3 physics_body_get_angular_velocity(const PhysicsBody* body);

/// Retrieve whether the given `body` is currently active or not.
NIKOLA_API const bool physics_body_is_active(const PhysicsBody* body);

/// Retrieve whether the given `body` is a sensor.
NIKOLA_API const bool physics_body_is_sensor(const PhysicsBody* body);

/// Retrieve internal user data of the given `body`.
NIKOLA_API void* physics_body_get_user_data(const PhysicsBody* body);

/// Retrieve the object layer of the given `body`.
NIKOLA_API const PhysicsObjectLayer physics_body_get_layer(const PhysicsBody* body);

/// Retrieve the restitution of the given `body`.
NIKOLA_API const f32 physics_body_get_restitution(const PhysicsBody* body);

/// Retrieve the friction of the given `body`.
NIKOLA_API const f32 physics_body_get_friction(const PhysicsBody* body);

/// Retrieve the gravity factor of the given `body`.
NIKOLA_API const f32 physics_body_get_gravity_factor(const PhysicsBody* body);

/// Retrieve the body type of the given `body`.
NIKOLA_API const PhysicsBodyType physics_body_get_type(const PhysicsBody* body);

/// Retrieve the transform of the given `body`.
NIKOLA_API Transform physics_body_get_transform(const PhysicsBody* body);

/// Retrieve the collider of the given `body`.
NIKOLA_API Collider* physics_body_get_collider(const PhysicsBody* body);

/// Physics body functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Collider functions

/// Create a box collider using the information provided in `desc`, returning 
/// back a valid `Collider`.
NIKOLA_API Collider* collider_create(const BoxColliderDesc& desc);

/// Create a sphere collider using the information provided in `desc`, returning 
/// back a valid `Collider`.
NIKOLA_API Collider* collider_create(const SphereColliderDesc& desc);

/// Create a capsule collider using the information provided in `desc`, returning 
/// back a valid `Collider`.
NIKOLA_API Collider* collider_create(const CapsuleColliderDesc& desc);

/// Retrieve the type of the given `collider`.
NIKOLA_API ColliderType collider_get_type(const Collider* collider);

/// Get extents of the given `collider`. 
///
/// @NOTE: Depending on the collider's type, the `Vec3` returned 
/// will have a different meaning. 
///
/// If the given `collider` is of type `COLLIDER_BOX`, the returned 
/// `Vec3` will just be the X, Y, and Z extents of that box. 
///
/// If the given `collider` is of type `COLLIDER_SPHERE`,  
/// X, Y, and Z of the returned `Vec3` will be the radius of that sphere.
///
/// If the given `collider` is of type `COLLIDER_CAPSULE`,  
/// X axis of the returned `Vec3` will be the radius of that capsule, 
/// while the Y-axis will be the half radius. The Z is unused.
NIKOLA_API Vec3 collider_get_extents(const Collider* collider);

/// Collider functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Character body functions

/// Create a character body using the information provided in the given `desc`, returning 
/// back a valid `Character`.
///
/// A "character" is a convenient physics body to be used for character movements in a game. 
NIKOLA_API Character* character_body_create(const CharacterBodyDesc& desc);

/// Destroy the given `character` and delete it from memory. 
///
/// @NOTE: Make sure to call `physics_world_remove_character` _before_ this function.
NIKOLA_API void character_body_destroy(Character* character);

/// This function _must_ be called for the character to simulate fucntionalities 
/// such as slop travesal and whatnot. This function _must_ be called _after_ 
/// `physics_world_update`.
NIKOLA_API void character_body_update(Character* character);

/// Set the position of the given `character` to `position`.
NIKOLA_API void character_body_set_position(Character* character, const Vec3 position);

/// Set the rotation of the given `character` to `rotation`.
NIKOLA_API void character_body_set_rotation(Character* character, const Quat rotation);

/// Set the rotation of the given `character` to using the `axis` and `angle`.
NIKOLA_API void character_body_set_rotation(Character* character, const Vec3 axis, const f32 angle);

/// Set the linear velocity of the given `character` to `velocity`.
NIKOLA_API void character_body_set_linear_velocity(Character* character, const Vec3 velocity);

/// Set the object layer of the given `character` to `layer`.
NIKOLA_API void character_body_set_layer(Character* character, const PhysicsObjectLayer layer);

/// Set the maximum slope angle of the given `character` to `max_slope_angle`.
NIKOLA_API void character_body_set_slope_angle(Character* character, const f32 max_slope_angle);

/// Set the collider of the given `character` to `collider`.
/// The given `max_penetration_depth` will be used to determine whether if `collider` is currently 
/// colliding with any other collider before switching.
///
/// @NOTE: The given `collider` _MUST_ be valid, otherwise the function will assert.
NIKOLA_API void character_body_set_collider(Character* character, const Collider* collider, const f32 max_penetration_depth);

/// Activate the given `character` body.
NIKOLA_API void character_body_activate(Character* character);

/// Apply linear velocity `velocity` to the given `character`. 
NIKOLA_API void character_body_apply_linear_velocity(Character* character, const Vec3 velocity);

/// Apply impulse force `impulse` to the given `character`. 
NIKOLA_API void character_body_apply_impulse(Character* character, const Vec3 impulse);

/// Retrieve the position of the given `character`.
NIKOLA_API const Vec3 character_body_get_position(const Character* character);

/// Retrieve the center of mass position of the given `character`.
NIKOLA_API const Vec3 character_body_get_com_position(const Character* character);

/// Retrieve the quaternion rotation of the given `character`.
NIKOLA_API const Quat character_body_get_rotation(const Character* character);

/// Retrieve the linear velocity of the given `character`.
NIKOLA_API const Vec3 character_body_get_linear_velocity(const Character* character);

/// Retrieve the object layer of the given `character`.
NIKOLA_API const PhysicsObjectLayer character_body_get_layer(const Character* character);

/// Retrieve the maximum slope angle of the given `character`.
NIKOLA_API const f32 character_body_get_slope_angle(const Character* character);

/// Retrieve the transform of the given `character`.
NIKOLA_API Transform character_body_get_transform(const Character* character);

/// Retrieve the current ground state of the given `character`.
NIKOLA_API GroundState character_body_query_ground_state(const Character* character);

/// Check whether the given `character` is currently supported by a surface.
NIKOLA_API const bool character_body_is_supported(const Character* character);

/// Check whether the slope at `surface_normal` is too steep for the given `character`. 
NIKOLA_API const bool character_body_is_slope_too_steep(const Character* character, const Vec3 surface_normal);

/// Retrieve the position of the ground body touching `character`.
NIKOLA_API const Vec3 character_body_get_ground_position(const Character* character);

/// Retrieve the normal of the ground body touching `character`.
NIKOLA_API const Vec3 character_body_get_ground_normal(const Character* character);

/// Retrieve the velocity of the ground body touching `character`.
NIKOLA_API const Vec3 character_body_get_ground_velocity(const Character* character);

/// Retrieve the body ID of the ground body touching `character`.
NIKOLA_API const PhysicsBody* character_body_get_ground_body(const Character* character);

/// Cast a ray using the information provided in `cast_ray` and check if it collides with 
/// the given `character`.
NIKOLA_API const bool character_body_cast_ray(const Character* character, const RayCastDesc& cast_desc);

/// Character body functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Physics ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
