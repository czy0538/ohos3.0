# 常见问题<a name="ZH-CN_TOPIC_0000001055049072"></a>

1.  是否存在一个全局变量，所有的页面都可以访问？

    当前框架中不存在所有Page都可以访问的全局变量。

2.  如何获取dom中的元素？

    通过ref属性获取dom中的元素，详细示例如下图所示；获取的元素只能使用它的方法，不能改变属性。

    ```
    <!--index.hml-->
    <div class="container">
       <!--指定组件的ref属性为animator-->
       <image-animator class="image-player" ref="ainmator" images="{{images}}" duration="1s" onclick="handleClick"></image-animator>
    </div>
    
    /* index.js */
    export default {
        data: {
            images:[
                {src:"common/frame1.png"},
                {src:"common/frame2.png"},
                {src:"common/frame3.png"}
            ]
        },
        handleClick(){
            //通过$refs属性获取对应的组件，在hml中，组件的ref属性要设置为animator
            const animator = this.$refs.animator;
            const state = animator.getState();
            if(state == "paused"){
                animator.resume();
            }else if(state == "stopped"){
                animator.start();
            }else{
                animator.pause();
            }
        }
    }
    ```

3.  如何在页面间传值？

    通过router.replace方法中的params参数来传递，参考代码如下：

    第一个页面传递数据：

    ```
    router.replace({
        uri:'pages/detail/detail',  //要跳转的页面uri
        params:{transferData:this.data}  //传递的数据，数据个数和名称开发者自己定义，
    });
    ```

    第二个界面接受数据：

    ```
    onInit(){
        const data = this.transferData;  //在onInit函数中接受传递的数据
    }  
    ```

4.  list如何滚动到某个item？

    通过list的scrollTo方法滚动到指定的item，参数是目标item的index。Index参数可以通过scrollend事件获取或者开发者指定。

5.  text支持多行吗？

    text支持多行。通过回车键换行或者是不设置text的高度属性，由控件自动根据内容换行。

6.  为什么控件不显示？

    **现象描述**

    开发者在hml文件中添加的控件无法显示

    **可能原因**

    -   未设置width和height值；
    -   样式设置错误。

    **处理步骤**

    \(1\)检查是否设置width和height值，组件必须显式设置width和height值；

    \(2\)检查组件的样式设置是否正确。

7.  如何实现页面滑动？

    实现页面滑动目前有三种方式：scroll（根组件大小超过屏幕的大小即自动实现scroll效果）、list、swiper。开发者可以参考开发文档查看三者的区别，并加以使用。

8.  Left、Top为什么不生效？

    除根节点外，Left、Top配合Stack组件使用才有效果。

9.  动态绑定为什么不生效？

    在进行绑定时，必须先将要绑定的对象或者对象的属性进行定义，不能先绑定后定义。

10. 如何实现相对定位和绝对定位？

    使用div、stack（top left属性）来实现相对和绝对定位。

11. 如何控制控件的显示与隐藏？

    通过display、show和if来控制控件的显示与隐藏。区别在于：if为false时，组件会从VDOM中移除，而show仅是渲染时不可见，组件依然存在于VDOM中。

12. 使用Margin时，有什么注意事项？

    Stack组件不支持其子组件设置margin属性。

13. 使用事件订阅时，有什么注意事项？

    在应用运行期间只存在一个page，所以router.replace跳转是先销毁前一个页面，然后在新创建一个界面。因此，如果涉及到事件订阅的页面，每次页面创建时要进行事件订阅，跳转离开界面前取消事件订阅。

14. 使用动态绑定时，有什么注意事项？

    过多的动态绑定会消耗较多的内存，若非业务需要，尽量不要使用太多的动态绑定。

15. swiper loop属性如何生效？

    去掉第一个组件或者去掉最后一个组件，剩余的长度大于swiper长度，loop生效。

16. 使用数组时，有什么注意事项？

    数组元素不宜过多，尽量避免对大数组进行频繁操作。


