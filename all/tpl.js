/*
    misc code
*/

var tpl=new Array();




Events:addEvent('domready', function() {

    new mooTemplater().init();
    
});







var mooTemplater = new Class({
	init: function() {
	    $$("#hidden","div.hidden").each(function(key) {
		this.prepare(key.getChildren("div")[0].id.replace('_div',''));
	    },this);
	},
	prepare: function(key,func,obj) {
	    this.title=this.maketitle(key);
	    this.key=key;
	    this.obj=obj;
	    this.func=func;
	    
	    if($(key+"_div")) {
			tpl[key]=new Array();
			tpl[key]['time']=$time;
			tpl[key]['html']=$(key+"_div").getParent().innerHTML;
			$(key+"_div").getParent().dispose();
			templ.html=tpl[key]['html'];
			if(typeof(func) == 'function') func();		
	    } else if($defined(tpl[key]) && $defined(tpl[key]['html'])) {
			templ.html=tpl[key]['html'];
			if(typeof(func) == 'function') func();
	    } else {
			if(!$defined(tpl[key]) || !$defined(tpl[key]['js']) || tpl[key]['time'] + 6 < $time) {
				this.req = new Request.HTML({
					url:'/js.lua?'+page+'_'+key,
					method:'get',
					onSuccess:this.onCompleteHandler.bind(this)
				}).get();
			} else {
				this.html=eval(tpl[this.key]['js'],this.obj);
				if(typeof(this.func) == 'function') this.func();
			}
		}
	},
	onCompleteHandler:function() {
		if(this.req.xhr.status == '200') {
	    		tpl[this.key]=new Array();
			tpl[this.key]['js']=this.req.xhr.responseText;
			this.html=eval(tpl[this.key]['js'],this.obj);
			if(typeof(this.func) == 'function') this.func();
		} else {
			this.html="error load";
			tpl[this.key]['js']='\'error load\';';
		}
	},
	fetch: function(){
	    return this.html;
	},
	maketitle: function(key) {
		if($defined(title[key])) {
			return title[key];
		} else {
			return key.substring(0,1).toUpperCase()+key.substring(1);
		}
	}
});

