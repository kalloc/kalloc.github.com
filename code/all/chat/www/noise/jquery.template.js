/*
 * jQuery Template Engine
 * Copyright(c) 2006, Kurt Mackey.
 * 
 * This code is licensed under BSD license. 
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Inspired by (and partially ported from) Jack Slocum's YAHOO.ext.DomHelper.Template
 * http://www.jackslocum.com/docs/YAHOO.ext.DomHelper.Template.html
 * http://www.jackslocum.com
 */

/**
 * Appends templated data to the current set of jQuery objects.
 * @param {String} template Template used to format objects.
 * @param {Object} data The data to populate specified template.  Can be an array.
 * @param {Hash} settings Options.  Set 'compiled' to true for compiled template.  Also takes a regex 'pattern' and a 'defaultFieldName'.
 * @return {jQuery}
 */
jQuery.fn.appendTemplated = function(template, data, settings){
	var tmp;
	settings = jQuery.extend({
		 compiled : false
	  }, settings || {});
	
	if(template instanceof jQuery.template){
		tmp = template;
	}else{
		tmp = new jQuery.template(template, settings)
	}

	if(settings.compiled && tmp.compiled){
		//this is good
	}else if(settings.compiled){
		tmp.compile();
	}

	var result = tmp.apply(data);

	return this.each(
		function(){ jQuery(this).html(jQuery(this).html() + result); }
		);
}

/**
* @class
* An HTML template
* @constructor
* @param {String} html HTML fragment
* @param {Hash} settings A set of options.  Takes a regex 'pattern' and a 'defaultFieldName'.
*/
jQuery.template = function(html, settings){
	this.html = html;
	this.settingsVal = settings || {};
}
jQuery.template.prototype = {
    /**
     * Returns an HTML fragment of this template with values from the specified object applied
     * @param {Object} json The object to pull values from. Can be an array or a single object
     * @return {String}
     */
	apply : function(json){
		var items = [];
		var currItem;
		if(json instanceof Array){
			items = json;
		}else{
			items.push(json);
		}
		var empty = '';
		var settings = this.settings();
        var fn = function(match, index){
			if(index == settings.defaultFieldName){
				return currItem;
			}
            if(typeof currItem[index] != 'undefined'){
                return currItem[index];
            }else{
                return empty;
            }
        }
		var results = [];
		var pattern = (this.compiled) ? '' : settings.pattern;
		for(i = 0; i < items.length; i++){
			if(this.compiled){
				results.push(this.compiled(items[i]));
			}else{
				currItem = items[i];
				results.push(this.html.replace(pattern, fn));
			}
		}
        return results.join('');
	},

	/**
     * Compiles the template into an internal function, eliminating the RegEx overhead
     */
	compile : function(){
		var html = this.html;
        var re = this.settings().pattern;
		var body = [];
        body.push("this.compiled = function(values){ return ");
        var result;
        var lastMatchEnd = 0;
        while ((result = re.exec(html)) != null){
            body.push("'", html.substring(lastMatchEnd, result.index), "' + ");
			var field = html.substring(result.index+2,re.lastIndex-1);
			if(field == this.settings().defaultFieldName){
				body.push("values + ");
			}else{
				body.push("values['", field , "'] + ");
			}
            lastMatchEnd = re.lastIndex;
        }
        body.push("'", html.substr(lastMatchEnd), "';};");
        eval(body.join(''));
	},
	settings : function(settings){
		if(settings){
			this.settingsVal = settings;
		}
		if(this.settingsVal){
			this._settings = jQuery.extend({
				 pattern: /\#\{([a-zA-Z][a-zA-Z0-9]*)\}/g,
				 defaultFieldName: 'item'
			  }, this.settingsVal || {});
			this.settingsVal = null;
		}
		return this._settings;
	}
}