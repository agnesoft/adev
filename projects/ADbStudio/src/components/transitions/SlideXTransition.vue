<template>
  <transition name="slide-x"  v-bind="$attrs" v-on="$listeners">
        <slot v-if="opened"></slot>
  </transition>
</template>

<script>
export default {
    name: "SlideXTransition",
    props: {
      opened: {
        type: Boolean,
        default: false
      }
    },
}
</script>

<style>

    .slide-x-enter-active {
        -moz-transition-duration: 0.4s;
        -webkit-transition-duration: 0.4s;
        -o-transition-duration: 0.4s;
        transition-duration: 0.4s;
        -moz-transition-timing-function: ease;
        -webkit-transition-timing-function: ease;
        -o-transition-timing-function: ease;
        transition-timing-function: ease;
    }

    .slide-x-leave-active {
        -moz-transition-duration: 0.4s;
        -webkit-transition-duration: 0.4s;
        -o-transition-duration: 0.4s;
        transition-duration: 0.4s;
    }

    .slide-x-enter-to, .slide-x-leave {
        max-width:var(--max-width,1000px); 
        overflow: hidden;
    }

    .slide-x-enter, .slide-x-leave-to {
        overflow: hidden;
        max-width: 0;
    }
    
</style>
