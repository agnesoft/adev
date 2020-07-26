<template>
    <footer class="main-footer">
        <ul class="copyright">
            <li>© 2020 ADbStudio</li>
            <li><a href="https://www.agnesoft.com">Agnesoft</a></li>
        </ul>
    </footer>
</template>

<script>
export default {
    name: "MainFooter",
}
</script>

<style scoped>
    .main-footer{
        background-color: var(--dark-color);
        color: var(--light-color);
    }
    .copyright{
        position: relative;
        display: flex;
        justify-content: space-between;
        padding: 0.5rem 1rem;
        margin: 0.5rem;
    }

    .copyright:before{ 
        display: block;
        content: "";
        position: absolute;
        top: -0.5rem;
        left: 0;
        height: 1px;
        width: 100%;
        background: var(--light-color);
        background: var(--gradient-underline);
    }
    .copyright a:hover{
        opacity: 0.7;
    }
    .main-footer a{
        color: var(--light-color);
    }
</style>
