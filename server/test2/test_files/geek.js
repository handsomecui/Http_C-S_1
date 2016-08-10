
var d_url = location.href;
var timer = null;

$(function(){
    $('.header_mini').delegate('.fa-align-justify', 'click', function(){
        $('.left_nav,.left_nav_top').toggleClass('show');
        newScroller();
        return false;
    });
    $('.defaultnav .nav_bar').delegate('.fa-angle-down', 'click', function(){
        $(this).parent('li').toggleClass('child-off');
        $(this).parent('li').find('dl,li').toggle('fast',function(){
            newScroller();
        });
        return false;
    });



    $('.link_pjax').mousedown(function(){
        d_url = location.href;
        $('.geek_list').removeClass('focus');
        $(this).parents('.geek_list').addClass('focus');
    });

    //点击全部主题
    $('.toggle-subnav').click(function(){
    	$(".left_nav,.left_nav_top").toggleClass('show_subnav');
        newScroller();
        return false;
    });

    $('body').delegate('.right_cont_loading,.close_right', 'click', function(){
        $('body').removeClass('show_cont');
        $('.right_cont_loading').hide().removeClass('active');
        $('.cmmde').appendTo('body');
        close_cmmde();
        window.history.replaceState(1,'title', d_url);
        $('.scrolltop_bar').addClass('for-list').removeClass('for-right');
        if($('.center_main').scrollTop() < 200){
            $('.scrolltop_bar').addClass('affix-top').removeClass('affix');
        }else{
            $('.scrolltop_bar').addClass('affix').removeClass('affix-top');
        }
        /*if($('.geek_list.focus').length > 0){
            $('.geek_list.focus').find('.link_pjax').eq(0).focus();
        }else{
            $('.geek_list').eq(0).addClass('focus').find('.link_pjax').eq(0).focus();
        }*/
    });
    $('.geek_list').delegate('.power_down', 'click', function(){
        var _this = $(this);
        var _newsid = $(this).attr("rel");
        $.getJSON("/service/news/blacklist_add?id=" + _newsid,function(data) {
            if (data.status == 1) {
                alert("降权操作成功，该文章不再出现在最热列表中");
                _this.parents('.geek_list').slideUp(390,function(){
                    $(this).remove()
                });
            }else{
                alert(data.error);
            }
        });
    });

    $('.add_user .fa-plus').click(function(){
        $(this).parents('.add_user').addClass('open');
    });
    $('.add_user .cancel').click(function(){
        $(this).parents('.add_user').removeClass('open');
    });
    $('.csdn-toolbar .btns > li').mouseout(function(){
        $(this).removeClass('on');
    });

    $('body').keydown(function(e){
        var key =  e.which;
        if(key == 27){
            close_cmmde();
            $('body').removeClass('show_cont');
            $('.right_cont_loading').hide();
            $('.cmmde').appendTo('body');
            window.history.replaceState(1,'title', d_url);
            if($('.geek_list.focus').length > 0){
                $('.geek_list.focus').find('.link_pjax').eq(0).focus();
            }else{
                $('.geek_list').eq(0).addClass('focus').find('.link_pjax').eq(0).focus();
            }
        }
        if(key == 13 && e.shiftKey){
            $('.footer_bar .reply.start_cmmde').trigger('click');
        }

        if(key == 38 && !$('body').hasClass('show_cont') && !$('body').hasClass('open_cmmde')){
            var vdd = $('.geek_list.focus').index();
            //console.log(vdd);
            $('.geek_list.focus').removeClass('focus');
            $('.geek_list').eq(vdd - 1).addClass('focus').find('.link_pjax').focus();
        }
        if(key == 40 && !$('body').hasClass('show_cont') && !$('body').hasClass('open_cmmde')){
            var vdd = $('.geek_list.focus').index();
            //console.log(vdd);
            $('.geek_list.focus').removeClass('focus');
            $('.geek_list').eq(vdd + 1).addClass('focus').find('.link_pjax').focus();
        }
    });




    if($('#comments').length > 0){
        $('.scrolltop_bar').addClass('in_viewer');
    }
    $('.scrolltop_bar .new_comment').click(function(){
        $('#newest .footer_bar .reply.start_cmmde').trigger('click');
    });
    $('.js-scroll-top').click(function(){
        $('body,html,.right_cont_body').animate({scrollTop:0},1000);
    });
    $('.js-list-top').click(function(){
        $('.center_main').animate({scrollTop:0},1000);
    });
    $('body').delegate('.markdown_views', 'mouseover', function(){
        $('.markdown_views a').attr('target','_blank');
    });




});

$(document).on('pjax:success', function() {	
    $('body').addClass('show_cont');
    $('.cmmde').appendTo('.right_cont');
    $('.right_cont_body').height($(window).height()).focus();
    $('.right_cont_loading').addClass('active');
    $('.scrolltop_bar').addClass('for-right affix-top').removeClass('for-list affix');
    if($('.detail_ad_top').length>0){
	    $('.detail_ad_t_l').html('<ins data-revive-zoneid="119" data-revive-id="8c38e720de1c90a6f6ff52f3f89c4d57"></ins>');
	    $('.detail_ad_t_r').html('<ins data-revive-zoneid="120" data-revive-id="8c38e720de1c90a6f6ff52f3f89c4d57"></ins>');
    }
    $('.detail_ad').html('<ins data-revive-zoneid="121" data-revive-id="8c38e720de1c90a6f6ff52f3f89c4d57"></ins>');    
});
$(document).on('pjax:start', function() {
    $('.right_cont_loading').show();
    
});



//$(document).on('pjax:success', function() {
//    $('body').addClass('show_cont');
//    $('.cmmde').appendTo('.right_cont');
//});
//$(document).on('pjax:beforeReplace', function(c,o) {
//    console.log(c);
//    console.log("----------");
//    console.log(o);
//    console.log("----------");
//});
//$(document).on('pjax:timeout', function(x,o) {
//    console.log(x);
//    console.log("----------");
//    console.log(o);
//    console.log("----------");
//});
//$(document).on('pjax:error', function(xhr, textStatus, error, options) {
//    console.log(xhr);
//    console.log("----------");
//    console.log(textStatus);
//    console.log("----------");
//    console.log(error);
//    console.log("----------");
//    console.log(options);
//    console.log("----------");
//
//});


//左侧菜单滚动条
var scroller  = null;
var scrollbar = null;
function newScroller() {
    if($('#leftnav_scr').length > 0){
        var winh  = window.innerHeight;
        scroller  = new jsScroller(document.getElementById("leftnav_scr"), 100, winh);
        scrollbar = new jsScrollbar (document.getElementById("leftnav_scr_bar"), scroller, true);
    }
}


window.onload = function () {
    newScroller();
};
window.onresize = function () {
    newScroller();
};

function jsScroller (o, w, h) {
    var self = this;
    var list = o.getElementsByTagName("div");
    for (var i = 0; i < list.length; i++) {
        if (list[i].className.indexOf("scroller_cont") > -1) {
            o = list[i]; // 以 o 为对象，将对象包含的class名为Scroller-cont的元素付给 对象 o
        }
    }

    //Private methods
    this._setPos = function (x, y) {
        if (x < this.viewableWidth - this.totalWidth)
            x = this.viewableWidth - this.totalWidth;
        if (x > 0) x = 0;
        if (y < this.viewableHeight - this.totalHeight)
            y = this.viewableHeight - this.totalHeight;
        if (y > 0) y = 0;
        this._x = x;
        this._y = y;
        with (o.style) {
            left = this._x +"px";
            top  = this._y +"px";
        }
    };
    //Public Methods
    this.reset = function () {
        this.content = o;
        this.totalHeight = o.offsetHeight;
        this.totalWidth  = o.offsetWidth;
        this._x = 0;
        this._y = 0;
        with (o.style) {
            left = "0px";
            top  = "0px";
        }
    };
    this.scrollBy = function (x, y) {
        this._setPos(this._x + x, this._y + y);
    };
    this.scrollTo = function (x, y) {
        this._setPos(-x, -y);
    };
    this.stopScroll = function () {
        if (this.scrollTimer) window.clearInterval(this.scrollTimer);
    };
    this.startScroll = function (x, y) {
        this.stopScroll();
        this.scrollTimer = window.setInterval(
            function(){ self.scrollBy(x, y); }, 40
        );
    };
    this.swapContent = function (c, w, h) {
        o = c;
        var list = o.getElementsByTagName("div");
        for (var i = 0; i < list.length; i++) {
            if (list[i].className.indexOf("scroller_cont") > -1) {
                o = list[i];
            }
        }
        if (w) this.viewableWidth  = w;
        if (h) this.viewableHeight = h;
        this.reset();
    };

    //variables
    this.content = o;
    this.viewableWidth  = w;
    this.viewableHeight = h;
    this.totalWidth  = o.offsetWidth;
    this.totalHeight = o.offsetHeight;

    //this.totalHeight = $(o).outerHeight();
    this.scrollTimer = null;
    this.reset();
};


function jsScrollbar (o, s, a, ev) {
    var self = this;

    this.reset = function () {

        //Arguments that were passed
        this._parent = o;
        this._src    = s;
        this.auto    = a ? a : false;
        this.eventHandler = ev ? ev : function () {};
        //Component Objects
        this._up   = this._findComponent("scroller_up", this._parent);
        this._down = this._findComponent("scroller_down", this._parent);
        this._yTrack  = this._findComponent("scroller_track", this._parent);
        this._yHandle = this._findComponent("scroller_handle", this._yTrack);
        //Height and position properties
        this._trackTop = findOffsetTop(this._yTrack);
        this._trackHeight  = this._yTrack.offsetHeight;
        this._handleHeight = this._yHandle.offsetHeight;
        this._x = 0;
        this._y = 0;
        //Misc. variables
        this._scrollDist  = 5;
        this._scrollTimer = null;
        this._selectFunc  = null;
        this._grabPoint   = null;
        this._tempTarget  = null;
        this._tempDistX   = 0;
        this._tempDistY   = 0;
        this._disabled    = false;
        this._ratio = ( this._src.totalHeight- this._src.viewableHeight)/(this._trackHeight - this._handleHeight);
        this._yHandle.ondragstart  = function () {return false;};
        this._yHandle.onmousedown = function () {return false;};
        if(window.event){
            this._addEvent(this._src.content, "mousewheel", this._scrollbarWheel);
        }//W3C
        else{
            this._addEvent(this._src.content, "DOMMouseScroll", this._scrollbarWheel);
        }

        this._removeEvent(this._parent, "mousedown", this._scrollbarClick);
        this._addEvent(this._parent, "mousedown", this._scrollbarClick);

        //this._src.reset();
        with (this._yHandle.style) {
            top  = "0px";
            left = "0px";
        }
        this._moveContent();

        if (this._src.totalHeight < this._src.viewableHeight) {
            this._disabled = true;
            this._yHandle.style.visibility = "hidden";
            if (this.auto) this._parent.style.visibility = "hidden";
        } else {
            this._disabled = false;
            this._yHandle.style.visibility = "visible";
            this._parent.style.visibility  = "visible";
        }

    };
    this._addEvent = function (o, t, f) {
        if (o.addEventListener) o.addEventListener(t, f, false);
        else if (o.attachEvent) o.attachEvent('on'+ t, f);
        else o['on'+ t] = f;
    };
    this._removeEvent = function (o, t, f) {
        if (o.removeEventListener) o.removeEventListener(t, f, false);
        else if (o.detachEvent) o.detachEvent('on'+ t, f);
        else o['on'+ t] = null;
    };
    this._findComponent = function (c, o) {
        var kids = o.childNodes;
        for (var i = 0; i < kids.length; i++) {
            if (kids[i].className && kids[i].className == c) {
                return kids[i];
            }
        }
    };
    //Thank you, Quirksmode
    function findOffsetTop (o) {
        var t = 0;
        if (o.offsetParent) {
            while (o.offsetParent) {
                t += o.offsetTop;
                o  = o.offsetParent;
            }
        }
        return t;
    };
    this._scrollbarClick = function (e) {
        if (self._disabled) return false;

        e = e ? e : event;
        if (!e.target) e.target = e.srcElement;

        if (e.target.className.indexOf("scroller_up") > -1) self._scrollUp(e);
        else if (e.target.className.indexOf("scroller_down") > -1) self._scrollDown(e);
        else if (e.target.className.indexOf("scroller_track") > -1) self._scrollTrack(e);
        else if (e.target.className.indexOf("scroller_handle") > -1) self._scrollHandle(e);

        self._tempTarget = e.target;
        self._selectFunc = document.onselectstart;
        document.onselectstart = function () {return false;};

        self.eventHandler(e.target, "mousedown");
        self._addEvent(document, "mouseup", self._stopScroll, false);

        return false;
    };
    this._scrollbarDrag = function (e) {
        e = e ? e : event;
        var t = parseInt(self._yHandle.style.top);
        var v = e.clientY + document.body.scrollTop - self._trackTop;
        with (self._yHandle.style) {
            if (v >= self._trackHeight - self._handleHeight + self._grabPoint)
                top = self._trackHeight - self._handleHeight +"px";
            else if (v <= self._grabPoint) top = "0px";
            else top = v - self._grabPoint +"px";
            self._y = parseInt(top);
        }

        self._moveContent();
    };
    this._scrollbarWheel = function (e) {
        e = e ? e : event;
        var dir = 0;
        if (e.wheelDelta >= 120) dir = -1;
        if (e.wheelDelta <= -120) dir = 1;
        if(e.detail >=3) dir= 1;
        if(e.detail <=-3) dir = -1;

         //console.log(self._src);
        self._src.totalHeight = $(".scroller_cont").innerHeight();  //重新赋值，把scroller的totalHeight去重新获取 2016-2-2
        self._ratio = ( self._src.totalHeight- self._src.viewableHeight)/(self._trackHeight - self._handleHeight);
        self.scrollBy(0, dir * 20);
        e.returnValue = false;

    };
    this._startScroll = function (x, y) {
        this._tempDistX = x;
        this._tempDistY = y;
        //console.log(this._src.totalHeight);
        this._scrollTimer = window.setInterval(function () {
            self.scrollBy(self._tempDistX, self._tempDistY);
        }, 40);
    };
    this._stopScroll = function () {
        self._removeEvent(document, "mousemove", self._scrollbarDrag, false);
        self._removeEvent(document, "mouseup", self._stopScroll, false);

        if (self._selectFunc) document.onselectstart = self._selectFunc;
        else document.onselectstart = function () { return true; };

        if (self._scrollTimer) window.clearInterval(self._scrollTimer);
        self.eventHandler (self._tempTarget, "mouseup");
    };
    this._scrollUp = function (e) {this._startScroll(0, -this._scrollDist);};
    this._scrollDown = function (e) {this._startScroll(0, this._scrollDist);};
    this._scrollTrack = function (e) {
        var curY = e.clientY + document.body.scrollTop;
        this._scroll(0, curY - this._trackTop - this._handleHeight/2);
    };
    this._scrollHandle = function (e) {
        var curY = e.clientY + document.body.scrollTop;
        this._grabPoint = curY - findOffsetTop(this._yHandle);
        this._addEvent(document, "mousemove", this._scrollbarDrag, false);
    };
    this._scroll = function (x, y) {
        if (y > this._trackHeight - this._handleHeight)
            y = this._trackHeight - this._handleHeight;
        if (y < 0) y = 0;

        this._yHandle.style.top = y +"px";
        this._y = y;

        this._moveContent();
    };
    this._moveContent = function () {
        this._src.scrollTo(0, Math.round(this._y * this._ratio));
    };

    this.scrollBy = function (x, y) {
        this._scroll(0, (-this._src._y + y)/this._ratio);
    };
    this.scrollTo = function (x, y) {
        this._scroll(0, y/this._ratio);
    };
    this.swapContent = function (o, w, h) {// 判断浏览器
        if(window.event){
            this._removeEvent(this._src.content, "mousewheel", this._scrollbarWheel, false);
        }//W3C
        else{
            this._removeEvent(this._src.content, "DOMMouseScroll", this._scrollbarWheel, false);
        }

        this._src.swapContent(o, w, h);
        this.reset();
    };

    this.reset();
};
//左侧菜单滚动条