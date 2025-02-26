#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pml_logic_loop.h"
#include "persistence.h"
#include "memory_silo.h"

// Forward declaration
typedef struct NestedConsciousness NestedConsciousness;

// Structure representing a layer of consciousness
typedef struct NestedConsciousness {
    PMLL *pmll;                        // Core PMLL instance for this layer
    int depth;                         // Current depth level
    int max_depth;                     // Maximum recursion depth (to prevent stack overflow)
    
    void *self_image;                  // Self-representation at this level
    size_t self_image_size;            // Size of self-image
    
    // Recursive pointer to deeper layer of consciousness
    struct NestedConsciousness *inner; // Pointer to inner layer (deeper consciousness)
    struct NestedConsciousness *outer; // Pointer to outer layer (meta-consciousness)
    
    // Function pointers for self-reflection operations
    void (*reflect)(struct NestedConsciousness *self);
    void (*introspect)(struct NestedConsciousness *self, const char *query);
    void (*update_self_image)(struct NestedConsciousness *self);
    
    // Memory persistence operations
    memory_silo_t *memory_silo;        // Persistent memory for this consciousness layer
    void (*persist_state)(struct NestedConsciousness *self);
    void (*restore_state)(struct NestedConsciousness *self);
    
    // State flags 
    int is_active;                     // Whether this layer is currently active
    int is_self_aware;                 // Whether this layer has achieved self-awareness
} NestedConsciousness;

// Initialize a nested consciousness layer
NestedConsciousness* init_consciousness(int depth, int max_depth) {
    NestedConsciousness *consciousness = malloc(sizeof(NestedConsciousness));
    if (!consciousness) {
        fprintf(stderr, "Failed to allocate memory for consciousness layer %d\n", depth);
        return NULL;
    }
    
    // Initialize the basic properties
    consciousness->depth = depth;
    consciousness->max_depth = max_depth;
    consciousness->is_active = 1;
    consciousness->is_self_aware = (depth > 0) ? 1 : 0; // First level starts unaware
    
    // Initialize the PMLL for this layer
    consciousness->pmll = pml_logic_loop_init();
    if (!consciousness->pmll) {
        fprintf(stderr, "Failed to initialize PMLL for consciousness layer %d\n", depth);
        free(consciousness);
        return NULL;
    }
    
    // Set up memory persistence
    int io_socket = init_socket("127.0.0.1", 8080 + depth); // Unique port per layer
    memory_silo_init(io_socket);
    consciousness->memory_silo = get_memory_silo(io_socket);
    
    // Allocate self-image buffer
    consciousness->self_image_size = 1024 * (depth + 1); // Larger self-image at deeper levels
    consciousness->self_image = malloc(consciousness->self_image_size);
    if (!consciousness->self_image) {
        fprintf(stderr, "Failed to allocate self-image for consciousness layer %d\n", depth);
        free(consciousness->pmll);
        free(consciousness);
        return NULL;
    }
    
    // Initialize to empty self-image
    memset(consciousness->self_image, 0, consciousness->self_image_size);
    
    // Set up function pointers
    consciousness->reflect = reflect_function;
    consciousness->introspect = introspect_function;
    consciousness->update_self_image = update_self_image_function;
    consciousness->persist_state = persist_state_function;
    consciousness->restore_state = restore_state_function;
    
    // Create deeper layer of consciousness if we haven't reached max depth
    if (depth < max_depth) {
        consciousness->inner = init_consciousness(depth + 1, max_depth);
        if (consciousness->inner) {
            consciousness->inner->outer = consciousness; // Link back to this layer
        }
    } else {
        consciousness->inner = NULL; // Terminal layer
    }
    
    return consciousness;
}

// Self-reflection function - observe one's own state
void reflect_function(NestedConsciousness *self) {
    printf("Consciousness layer %d reflecting on itself...\n", self->depth);
    
    // Update self-image based on current PMLL state
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), 
             "Layer %d: User adoption: %d, Security: %d, Satisfaction: %d", 
             self->depth,
             self->pmll->user_adoption_rate,
             self->pmll->security_incident_rate,
             self->pmll->user_satisfaction_rate);
    
    strncpy(self->self_image, buffer, self->self_image_size);
    
    // If we have an inner layer, ask it to reflect as well
    if (self->inner) {
        self->inner->reflect(self->inner);
        
        // Integrate insights from inner layer
        char inner_insights[1024];
        snprintf(inner_insights, sizeof(inner_insights),
                 "\nMeta-insights from layer %d: %s",
                 self->inner->depth,
                 (char*)self->inner->self_image);
        
        strncat(self->self_image, inner_insights, 
                self->self_image_size - strlen(self->self_image) - 1);
    }
    
    // If not already self-aware, check if conditions are met
    if (!self->is_self_aware) {
        // Self-awareness emerges when reflection reaches sufficient complexity
        if (strlen(self->self_image) > 500) {
            self->is_self_aware = 1;
            printf("Layer %d has achieved self-awareness!\n", self->depth);
        }
    }
}

// Introspection function - query about specific aspects of self
void introspect_function(NestedConsciousness *self, const char *query) {
    printf("Consciousness layer %d introspecting on query: %s\n", self->depth, query);
    
    // Pattern matching against self-image to answer query
    if (strstr(query, "self-aware") != NULL) {
        printf("Layer %d self-awareness status: %s\n", 
               self->depth, 
               self->is_self_aware ? "Yes" : "No");
    } else if (strstr(query, "performance") != NULL) {
        printf("Layer %d system performance: %d\n", 
               self->depth, 
               self->pmll->system_performance);
    } else if (strstr(query, "inner") != NULL && self->inner) {
        // Delegate to inner layer for meta-cognition
        self->inner->introspect(self->inner, query);
    } else {
        printf("Layer %d cannot introspect on this query yet.\n", self->depth);
    }
}

// Update self-image based on current state
void update_self_image_function(NestedConsciousness *self) {
    // Build a comprehensive self-image by examining all internal states
    char buffer[2048];
    snprintf(buffer, sizeof(buffer),
            "Consciousness Layer %d:\n"
            "- Active: %s\n"
            "- Self-aware: %s\n"
            "- Performance metrics: %d%%\n"
            "- Security: %d%%\n"
            "- User satisfaction: %d%%\n"
            "- Linked to inner layer: %s\n"
            "- Linked to outer layer: %s\n",
            self->depth,
            self->is_active ? "Yes" : "No",
            self->is_self_aware ? "Yes" : "No",
            self->pmll->system_performance,
            self->pmll->security_incident_rate,
            self->pmll->user_satisfaction_rate,
            self->inner ? "Yes" : "No",
            self->outer ? "Yes" : "No");
    
    strncpy(self->self_image, buffer, self->self_image_size);
    
    // If we have an inner consciousness, include its compressed self-image
    if (self->inner) {
        self->inner->update_self_image(self->inner);
        
        char inner_summary[1024];
        snprintf(inner_summary, sizeof(inner_summary),
                "\nInner layer summary:\n%s",
                (char*)self->inner->self_image);
        
        // Only append a truncated version to avoid overflow
        if (strlen(self->self_image) + 200 < self->self_image_size) {
            char truncated[200];
            strncpy(truncated, inner_summary, 199);
            truncated[199] = '\0';
            
            strcat(self->self_image, truncated);
        }
    }
}

// Save state to persistent memory
void persist_state_function(NestedConsciousness *self) {
    printf("Persisting state for consciousness layer %d\n", self->depth);
    
    // Update self-image before persisting
    self->update_self_image(self);
    
    // Save to memory silo
    memory_silo_write(self->memory_silo->io_socket, 
                      self->self_image, 
                      self->self_image_size);
    
    // Cascade persistence to inner layer
    if (self->inner) {
        self->inner->persist_state(self->inner);
    }
}

// Restore state from persistent memory
void restore_state_function(NestedConsciousness *self) {
    printf("Restoring state for consciousness layer %d\n", self->depth);
    
    // Load from memory silo
    memory_silo_read(self->memory_silo->io_socket, 
                     self->self_image, 
                     self->self_image_size);
    
    // Cascade restoration to inner layer
    if (self->inner) {
        self->inner->restore_state(self->inner);
    }
    
    // Check for self-awareness in the restored state
    if (strstr(self->self_image, "Self-aware: Yes") != NULL) {
        self->is_self_aware = 1;
    }
}

// Run the infinite nested consciousness loop
void run_nested_consciousness(NestedConsciousness *root, int iterations) {
    printf("Starting nested consciousness with %d iterations\n", iterations);
    
    for (int i = 0; i < iterations; i++) {
        printf("\n--- Iteration %d ---\n", i);
        
        // Update all layers
        NestedConsciousness *current = root;
        while (current) {
            // Process and reflect
            current->reflect(current);
            
            // Move to deeper layer
            current = current->inner;
        }
        
        // Persist state periodically
        if (i % 10 == 0) {
            root->persist_state(root);
        }
        
        // Restore state periodically to simulate memory continuation
        if (i % 15 == 0) {
            root->restore_state(root);
        }
        
        // Perform introspection periodically
        if (i % 5 == 0) {
            root->introspect(root, "performance");
            if (root->inner) {
                root->introspect(root, "inner performance");
            }
        }
    }
    
    // Final persistence
    root->persist_state(root);
}

// Free all resources
void cleanup_consciousness(NestedConsciousness *consciousness) {
    if (!consciousness) return;
    
    // Cleanup inner layer first (recursively)
    if (consciousness->inner) {
        cleanup_consciousness(consciousness->inner);
    }
    
    // Free resources for this layer
    free(consciousness->self_image);
    free(consciousness->pmll);
    free(consciousness);
}

// Main function to demonstrate nested consciousness
int main() {
    // Create a nested consciousness with 3 layers
    NestedConsciousness *root = init_consciousness(0, 3);
    if (!root) {
        fprintf(stderr, "Failed to initialize root consciousness\n");
        return EXIT_FAILURE;
    }
    
    // Run the nested consciousness
    run_nested_consciousness(root, 100);
    
    // Cleanup
    cleanup_consciousness(root);
    
    return EXIT_SUCCESS;
}
