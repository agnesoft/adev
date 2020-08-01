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
        -webkit-box-shadow:  var(--dark-color) 1px 1px 2px 0px;
        -moz-box-shadow:  var(--dark-color) 1px 1px 2px 0px;
        box-shadow:  var(--dark-color) 1px 1px 2px 0px;
    }
    .btn-primary{
        color: var(--light-color);
        background-color: var(--primary-color);
        transition: var(--transition-opacity);
    }
    .btn-primary:hover{
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
</style>
