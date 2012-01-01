Ext.chart.Chart.CHART_URL = 'ext-3.3/resources/charts.swf';
var chartHeight = 140;

var processStore = new Ext.data.JsonStore({
       fields: pFields,
       data: pData
});

var fileStore = new Ext.data.JsonStore({
       fields:fFields,
       data: fData
});

var registryStore = new Ext.data.JsonStore({
       fields:rFields,
       data: rData
});

var networkStore = new Ext.data.JsonStore({
       fields:nFields,
       data: nData
});

 // extra extra simple
 var pChart = new Ext.Panel({
       width:500,
       height: chartHeight,
       minSize: 100,
       maxSize: 400,
       margins: '2 2 0 2',
       cmargins: '2 2 0 2',
       title: 'API Calling Visualize, 2011',
       collapsible: true,
       split: true,
       region: 'north',
       items: {
              xtype: 'stackedbarchart',
              store: processStore,
              yField: 'tab',
              xAxis: new Ext.chart.NumericAxis({
                       stackingEnabled: true
                       //labelRenderer: Ext.util.Format.usMoney
                }),
              series: [{
                        xField: 'dll',
                        displayName: 'DLL Loaded'
              },{
                        xField: 'file',
                        displayName: 'File and Process APIs Called'
              },{
                        xField: 'registry',
                        displayName: 'Registry APIs Called'
              },{
                        xField: 'network',
                        displayName: 'Network APIs Called'
              }]
       }
    });

 // extra extra simple
 var fChart =  new Ext.Panel({
        width:500,
        height: chartHeight,
	minSize: 100,
	maxSize: 400,
	margins: '2 2 0 2',
        cmargins: '2 2 0 2',
        title: 'API Calling Visualize',
	collapsible: true,
	split: true,
	region: 'north',
        items: {
            xtype: 'columnchart',
            store: fileStore,
            xField: 'name',
            yField: 'call',
			listeners: {
				itemclick: function(o){
					var rec = store.getAt(o.index);
					Ext.example.msg('Item Selected', 'You chose {0}.', rec.get('name'));
				}
			}
        }
    });
 ////////////
 var rChart =  new Ext.Panel({
       width:500,
       height: chartHeight,
       minSize: 100,
       maxSize: 400,
       margins: '2 2 0 2',
       cmargins: '2 2 0 2',
       title: 'API Calling Visualize',
       collapsible: true,
       split: true,
       region: 'north',
       items: {
           xtype: 'columnchart',
           store: registryStore,
           xField: 'name',
           yField: 'call',
             listeners: {
                     itemclick: function(o){
                           var rec = store.getAt(o.index);
                           Ext.example.msg('Item Selected', 'You chose {0}.', rec.get('name'));
                    }
             }
      }
});
 ////////////
 var nChart =  new Ext.Panel({
        width:500,
        height: chartHeight,
	minSize: 100,
	maxSize: 400,
	margins: '2 2 0 2',
        cmargins: '2 2 0 2',
        title: 'API Calling Visualize',
	collapsible: true,
	split: true,
	region: 'north',
        items: {
            xtype: 'columnchart',
            store: networkStore,
            xField: 'name',
            yField: 'call',
              listeners: {
                     itemclick: function(o){
                            var rec = store.getAt(o.index);
                            Ext.example.msg('Item Selected', 'You chose {0}.', rec.get('name'));
                     }
              }
        }
    });
