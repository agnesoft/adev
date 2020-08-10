<template>
    <FadeTransition :opened="opened">   
        <div class="base-message">
            <div class="message">   
                <slot></slot>
            </div>
            <BaseButton class="close" v-if="closable" @click="handleClick">
                <BaseIcon icon_key="close" />
            </BaseButton>
        </div>
    </FadeTransition>
</template>

<script>
import FadeTransition from "@/components//transitions/FadeTransition";
export default {
    name: "BaseMessage",
    props: {
        closable: {
            type: Boolean,
            default: true,
        },
        opened: {
            type: Boolean,
            default: true,
        },
    },
    components: {
        FadeTransition
    },
    methods: {
        handleClick(){
            this.$emit('close');
        },
    }
}
</script>

<style scoped>
    .base-message{
        --button-size: 1.8rem;
        border-radius: calc(var(--button-size) / 2);
        border: 1px solid grey;
        background: lightgrey;
        --shadow: var(--dark-color) 1px 1px 5px 2px;
        -webkit-box-shadow:  var(--shadow);
        -moz-box-shadow:  var(--shadow);
        box-shadow:  var(--shadow);
        display: flex;
        align-items: center;
    }
    .message{
        flex-grow: 2;
        /* line-height: var(--button-size); */
        padding: 0.4rem 1rem ;
        font-size: 0.8rem;
    }
    .close{
        width: var(--button-size);
        height: var(--button-size);
        padding: 0.2rem;
        transition: var(--transition-opacity);
        background: none;
        --shadow:  none; 
    }
    .close:hover{
        opacity: 0.7;
    }
    .error{
        border-color: rgb(233, 18, 2);
        background: rgb(238, 60, 48);
        color: white;
        --icon-color: white;
    }
    .info{
        border-color: var(--primary-color);
        background: var(--light-color);
        color: var(--dark-color);
        /* --icon-color: var(--primary-color); */
        --icon-color: var(--dark-color);
    }
</style>
