<template>
  <transition name="slide-y"  v-bind="$attrs" v-on="$listeners">
        <slot v-if="opened"></slot>
  </transition>
</template>

<script>
export default {
    name: "SlideYTransition",
    props: {
      opened: {
        type: Boolean,
        default: false
      }
    },
}
</script>

<style>

    .slide-y-enter-active {
        -moz-transition-duration: 0.4s;
        -webkit-transition-duration: 0.4s;
        -o-transition-duration: 0.4s;
        transition-duration: 0.4s;
        -moz-transition-timing-function: ease;
        -webkit-transition-timing-function: ease;
        -o-transition-timing-function: ease;
        transition-timing-function: ease;
    }

    .slide-y-leave-active {
        -moz-transition-duration: 0.4s;
        -webkit-transition-duration: 0.4s;
        -o-transition-duration: 0.4s;
        transition-duration: 0.4s;
    }

    .slide-y-enter-to, .slide-y-leave {
        max-height:var(--max-height,1000px); 
        overflow: hidden;
    }

    .slide-y-enter, .slide-y-leave-to {
        overflow: hidden;
        max-height: 0;
    }
    
</style>
