var currheight;
var currwidth;

function init_index(){
    
    $('load').set('tween',{
	duration:1500,
	onComplete:function() {
	    if(js == 'login_error' && $('login_error')) $('login_error').fade('show');
	    document.title=document.title.replace(/ \([^)]+\)$/,"");

	    new MochaUI.Window({
			        id: 'mywindow',
			        title: 'Welcome to Well',
			        content: $("login_form"),
					restrict:false,
					collapsible:false,
					minimizable:false,
					maximizable:false,
					closable:false,
					draggable:false,
					resizable:false,
			        width: 340,
			        height: 150
		});
	

	    $('login_form').addEvent('submit', function(e) {
			e.stop();
			var request = new Request.JSON({
			url: '/',
			data: {login:$('login_input').value},
			onComplete: function(json) {
					if($defined(json)) {
						if($defined(json.content)) $('content').innerHTML=json.content;
						if($defined(json.js)) eval(json.js);
					}
				}
			}).send();
	    });
		(function () { $("login_input").focus() }).delay(1000);
	}
    }).fade('out');

    
    
}




function init_main(){
    
    $('load').set('tween',{
	duration:1500,
	onComplete:function() {
	    if($('main'))   $('main').fade('show') ;

	    if (MochaUI) {


			$("char_info").makeResizable({
					modifiers: { 
							x:false,
							y:"height"
					},
					limit: {
						y:[300,size().height-300]
					},
					handle:"handle1",
					onDrag: function() {
						$("chat_user").setStyle("height",size().height-$("char_info").clientHeight-10);
						$("char_info").setStyle("height",size().height-$("chat_user").clientHeight-10);
					},
					onComplete:function() {
						$("chat_user").setStyle("height",size().height-$("char_info").clientHeight-10);
						$("char_info").setStyle("height",size().height-$("chat_user").clientHeight-10);
					}
			});
			
			$("userlist").makeResizable({
					invert:true,
					modifiers: { 
							x:"width",
							y:false
					},
					limit:	{
						x:[200,400]
					},
					handle:"handle2",
					onDrag: function() {
						$("chat").setStyle("width",size().width-$("userlist").clientWidth-28);
					},
					onBeforeStart:function() {
						$("userlist").setStyle("right",0);

					},
					onComplete:function() {
						$("chat").setStyle("width",size().width-$("userlist").clientWidth-28);
					}
			});
			

            $("handle2").setStyles({'cursor': Browser.Engine.presto ? 'e-resize' : 'col-resize'});
            $("handle1").setStyles({'cursor': Browser.Engine.presto ? 'n-resize' : 'row-resize'});

		}

	    document.title=document.title.replace(/ \([^)]+\)$/,"");
	}
    }).fade('out');



}

function resize_index () {

	if($("mywindow")) {
		$("mywindow").setStyle("left",size().width/2-$("mywindow").clientWidth/2);
		$("mywindow").setStyle("top",(size().height/2)-($("mywindow").clientHeight/2));

	}

}



function resize_main() {

	$("main").setStyle("width",size().width);
	$("main").setStyle("height",size().height);


	$("chat").setStyle("width",size().width-$("userlist").clientWidth-20);

	$("info").setStyle("width",size().width-$("char").clientWidth-20);
	if(size().height-$("chat_user").clientHeight-10 < 300) {
		$("char_info").setStyle("height",300);
	} else {
		$("char_info").setStyle("height",size().height-$("chat_user").clientHeight-10);
	}
 


	

}










Events:addEvent('domready', function() {
    if(typeof(resize) == "string" && typeof(window[resize]) == "function") {

		window.addEvent('resize', function() {
			if(currwidth != document.documentElement.offsetWidth || currheight != document.documentElement.offsetHeight) {
				window[resize]();
				currwidth=document.documentElement.offsetWidth;
				currheight=document.documentElement.offsetHeight;
			}
		} );
		window[resize]();
    }
    if(typeof(init) == "string" && typeof(window[init]) == "function") {
		window[init]();
	}
});



window.size = function() {
	var w = 0;
	var h = 0;
	if(!window.innerWidth)	{if(!(document.documentElement.offsetWidth == 0)){w = document.documentElement.offsetWidth;h = document.documentElement.offsetHeight;}else{w = document.body.offsetWidth;h = document.body.offsetHeight;}}
	else	{w = window.innerWidth;h = window.innerHeight;}
	return {width:w,height:h};
}







