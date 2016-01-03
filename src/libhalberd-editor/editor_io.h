#include "actor.h"

//-----------------------------------------------------------------------------
// Init/Destruction
//-----------------------------------------------------------------------------

bool init_resource_loader();
void destroy_resource_loader();

//-----------------------------------------------------------------------------
// Custom loaders
//-----------------------------------------------------------------------------

/*!
 * Gets callback information from an actor file. Also registers the current
 * actor/swaps it with the previously loaded one, so that it can be destroyed
 * later.
 */
bool register_actor_callbacks(xmlNodePtr ptr, actor a, const char* resource_location, const char* resource_name);

/*!
 * Registers an actor.
 * If an actor is already registered, the previous actor is freed from memory.
 */
void register_actor(actor a, const char* resource_location, const char* resource_name);

/*!
 * Creates and registers a new actor. Fields are set to sane defaults.
 * See get_extended_resource_path(io_util.h) for usage details
 */
actor create_blank_actor_for_resource(const char* resource_location, const char* resource_name);

/*!
 * Saves the registered actor.
 */
void save_actor();

// TODO: Need an easy way to retrieve actor callback information
