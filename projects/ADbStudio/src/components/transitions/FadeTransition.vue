<template>
  <transition name="fade"  v-bind="$attrs" v-on="$listeners">
        <slot v-if="opened"></slot>
  </transition>
</template>

<script>
export default {
    name: "FadeTransition",
    props: {
      opened: {
        type: Boolean,
        default: false
      }
    },
}
</script>

<style>

    
    .fade-enter-active {
        transition: all 0.3s
    }
    .fade-enter {
        opacity: 0;
    }
    .fade-leave-active {
        transition: all 0.3s;
        opacity: 0;
    }
    
</style>
