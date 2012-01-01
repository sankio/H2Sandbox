/*!
 * Ext JS Library 3.3.1
 * Copyright(c) 2006-2010 Sencha Inc.
 * licensing@sencha.com
 * http://www.sencha.com/license
 */

//
// Extend the XmlTreeLoader to set some custom TreeNode attributes specific to our application:
//
//

var path = "static/data/" + sha;
var processPath = path + "process.xml";
var filePath = path + "file.xml";
var regPath = path + "reg.xml";
var networkPath = path + "network.xml";


Ext.app.FileLoader = Ext.extend(Ext.ux.tree.XmlTreeLoader, {
    processAttributes : function(attr){
        if(attr.category){ // is it an author node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            attr.text = attr.call + ' on ' + attr.time;

            // Author icon, using the gender flag to choose a specific icon:
            attr.iconCls = 'author-' + attr.gender;

            // Override these values for our folder nodes because we are loading all data at once.  If we were
            // loading each node asynchronously (the default) we would not want to do this:
            attr.loaded = true;
            attr.expanded = true;
        }
        else if(attr.name){ // is it a paras node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            attr.text = attr.name + ' : ' + attr.text;

            // Book icon:
            attr.iconCls = 'book';

            // Tell the tree this is a leaf node.  This could also be passed as an attribute in the original XML,
            // but this example demonstrates that you can control this even when you cannot dictate the format of
            // the incoming source XML:
            attr.leaf = true;
        }
    }
});

Ext.app.NetworkLoader = Ext.extend(Ext.ux.tree.XmlTreeLoader, {
    processAttributes : function(attr){
        if(attr.call){ // is it an author node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            attr.text = attr.call;

            // Author icon, using the gender flag to choose a specific icon:
            attr.iconCls = 'author-' + attr.gender;

            // Override these values for our folder nodes because we are loading all data at once.  If we were
            // loading each node asynchronously (the default) we would not want to do this:
            attr.loaded = true;
            attr.expanded = true;
        }
        else if(attr.name){ // is it a paras node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            attr.text = attr.name + ': ' + attr.text;

            // Book icon:
            attr.iconCls = 'book';

            // Tell the tree this is a leaf node.  This could also be passed as an attribute in the original XML,
            // but this example demonstrates that you can control this even when you cannot dictate the format of
            // the incoming source XML:
            //attr.loaded = true;
            //attr.expanded = true;
            attr.leaf = true;
        }
    }
});

Ext.app.ProcessLoader = Ext.extend(Ext.ux.tree.XmlTreeLoader, {
    processAttributes : function(attr){
        if(attr.name){ // is it an author node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            //attr.text = attr.call + ' on ' + attr.time;

            // Author icon, using the gender flag to choose a specific icon:
            //attr.iconCls = 'author-' + attr.gender;

            // Override these values for our folder nodes because we are loading all data at once.  If we were
            // loading each node asynchronously (the default) we would not want to do this:
            attr.loaded = true;
            attr.expanded = false;
            attr.text = attr.name + ": " + attr.text;
        }
        
        else if(attr.type){
        	attr.loaded = true;
            attr.expanded = false;
        }
        else if(attr.link){ // is it a paras node?

            // Set the node text that will show in the tree since our raw data does not include a text attribute:
            attr.text = attr.link + ', ' + "(" + attr.size + " bytes), was successfully loaded at address: 0x" + attr.start;

            // Book icon:
            attr.iconCls = 'book';

            // Tell the tree this is a leaf node.  This could also be passed as an attribute in the original XML,
            // but this example demonstrates that you can control this even when you cannot dictate the format of
            // the incoming source XML:
            attr.leaf = true;
        }
    }
});

Ext.onReady(function(){
    
    var detailsText = '<i>Select a parameter node to see more information...</i>';
    var processDetailsText = '<i>Select a parameter node to see more information...</i>';

	var tpl = new Ext.Template(
		'File Name: {filename}',
		'File Size: {filesize}&nbspBytes.',
		'Start Time: {starttime}',
		'Termination Time: {stoptime}',
		'Start Reason: AnalysisTarget',
		'Termination Reason: {stopreason}',
		'Execution Status: {runstatus}',
		'Application Type: Win32Application',
        '<h2 class="title">{title}</h2>',
        '<p><b>Parameter name</b>: {published}</p>',
        '<p><b>Value</b>: {innerText}</p>',
        '<p><a href="{url}" target="_blank">MSDN Link</a></p>'
	);
    tpl.compile();
    //
    
    var processText = new Ext.Template(
		'File Name: {filename}',
		'File Size: {filesize} Bytes.',
		'Start Time: {starttime}',
		'Termination Time: {stoptime}',
		'Start Reason: AnalysisTarget',
		'Termination Reason: {stopreason}',
		'Execution Status: {runstatus}',
		'Application Type: Win32Application',
        '<h2 class="title">{title}</h2>',
        '<p><b>Parameter name</b>: {published}</p>',
        '<p><b>Value</b>: {innerText}</p>',
        '<p><a href="{url}" target="_blank">MSDN Link</a></p>'
	);
    processText.compile();
    var processtree = {
            xtype: 'treepanel',
            id: 'tree-panel-process-detail',
	    title: 'Process Scanning Summary',
            region: 'center',
            height: 200,
            margins: '2 2 0 2',
            autoScroll: true,
	        rootVisible: false,
	        root: new Ext.tree.AsyncTreeNode(),

            // Our custom TreeLoader:
	        loader: new Ext.app.ProcessLoader({
	            dataUrl: processPath
	        })
        }
       
	
	///////////////////////////////
	
    var filetree = {
            xtype: 'treepanel',
            id: 'tree-panel',
	    title: 'File and Process APIs Called',
            region: 'center',
            margins: '2 2 0 2',
            autoScroll: true,
	        rootVisible: false,
	        root: new Ext.tree.AsyncTreeNode(),

            // Our custom TreeLoader:
	        loader: new Ext.app.FileLoader({
	            dataUrl:filePath
	        }),

	        listeners: {
	            'render': function(tp){
                    tp.getSelectionModel().on('selectionchange', function(tree, node){
                        var el = Ext.getCmp('details-panel-file').body;
	                    if(node && node.leaf){
	                        tpl.overwrite(el, node.attributes);
	                    }else{
                            el.update(detailsText);
                        }
                    })
	            }
	        }
        }
		
	/////////////////////////////////////
	var registrytree = {
            xtype: 'treepanel',
            id: 'tree-panel-reg',
            region: 'center',
	    layout: 'fit',
	    title: 'Registry APIs Called',
            margins: '2 2 0 2',
            autoScroll: true,
	        rootVisible: false,
	        root: new Ext.tree.AsyncTreeNode(),

            // Our custom TreeLoader:
	        loader: new Ext.app.FileLoader({
	            dataUrl:regPath
	        }),

	        listeners: {
	            'render': function(tp){
                    tp.getSelectionModel().on('selectionchange', function(tree, node){
                        var el = Ext.getCmp('details-panel-reg').body;
	                    if(node && node.leaf){
	                        tpl.overwrite(el, node.attributes);
	                    }else{
                            el.update(detailsText);
                        }
                    })
	            }
	        }
        }	
	///////////////
	var networktree = {
            xtype: 'treepanel',
            id: 'tree-panel-net',
	    title: 'Network APIs Called',
            region: 'center',
	    layout: 'fit',
            margins: '2 2 0 2',
            autoScroll: true,
	        rootVisible: false,
	        root: new Ext.tree.AsyncTreeNode(),

            // Our custom TreeLoader:
	        loader: new Ext.app.NetworkLoader({
	            dataUrl:networkPath
	        }),

	        listeners: {
	            'render': function(tp){
                    tp.getSelectionModel().on('selectionchange', function(tree, node){
                        var el = Ext.getCmp('details-panel-reg').body;
	                    if(node && node.leaf){
	                        tpl.overwrite(el, node.attributes);
	                    }else{
                            el.update(detailsText);
                        }
                    })
	            }
	        }
        }
        
    ////////////////
    var u = new Ext.TabPanel({
	id: 'tabs-nested-layouts-panel',
	title: 'TabPanel with Nested Layouts',
	region:'center',
	activeTab: 0,
	items:[{
		title: 'General',
		layout: 'border',
		items: [pChart,processtree]
	},{
		title: 'Process & File',
		layout: 'border',
		items: [fChart, filetree]
	},{
		title: 'Registry',
		layout: 'border',
		items: [rChart, registrytree]
	},{
		title: 'Network',
		layout: 'border',
		items: [nChart, networktree]
	}]
    });
    
    var vp=new Ext.Viewport({
	layout:'border',
	id:'view1',
	margin: '30 0 0 0',
	items:[
	    {
		region: 'north',
		split:false,
		width: '100%',
		height: 42,
		collapsible: false,
		html: '<div class="header"><div class="logo"></div></div>'
	    },
	    u
	    ]
    });
});
