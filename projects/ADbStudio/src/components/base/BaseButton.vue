<template>
    <button class="btn" @click="handleClick">
        <slot></slot>
    </button>
</template>

<script>
export default {
    name: "BaseButton",
    methods: {
        handleClick(){
            this.$emit('click');
        },
    }
}
</script>

<style>
    .btn{
        cursor: pointer;
        overflow: hidden;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 1rem;
        border: none;
        border-radius: 0.2rem;
        --shadow: var(--dark-color) 1px 1px 2px 0px;
        -webkit-box-shadow:  var(--shadow);
        -moz-box-shadow:  var(--shadow);
        box-shadow:  var(--shadow);
    }
    .btn-primary{
        color: var(--light-color);
        background-color: var(--primary-color);
        transition: var(--transition-opacity);
    }
    .btn-primary:hover, .btn-light:hover{
        opacity: 0.7;
    }
    .btn-default{
        border: 1px solid var(--dark-color);
        background-color: var(--light-grey);
        transition: filter 0.4s ease;
    }
    .btn-default:hover{
        filter: brightness(85%)
    }

    .btn-circle{
        border-radius: 50%;
        padding: calc(var(--button-size) * 0.15);
        width: var(--button-size);
        height: var(--button-size);
    }
    .btn-flat{        
        --shadow:  none;   
         border: solid var(--border-width) var(--light-color);
        background-color: var(--primary-color);
        color: var(--light-color);
        padding: calc(var(--button-size) * 0.1);
        font-size: calc(var(--button-size) * 0.6);
        line-height: calc(var(--button-size) * 0.6);
    }
    .btn-light{
        transition: var(--transition-opacity);
        --icon-color: var(--primary-color);
        background: none;
        --shadow:  none; 
    }

</style>
