import * as React from 'react';
import { StyleSheet } from 'react-native';
import { Text, View } from '../components/Themed';

import moment from 'moment'

export interface Props {

}
export interface State {
  error: string;
  isLoaded: boolean;
  temp: string;
  humid: string;
  duration: string;
  millis: string;
}

export default class TabOneScreen extends React.Component<Props, State> {
  constructor(props: Props) {
    super(props);
    this.state = {
      error: '',
      isLoaded: false,
      temp: '',
      humid: '',
      duration: '',
      millis: ''
    };
  }
  async getData(){
    fetch('http://10.0.0.228/sensor',{
      method: "GET",
      headers: {
        "Content-Type": "text/plain"
      }
    })
    .then(res => res.text())
    .then(result => {
      let split = result.split('\n')
      this.setState({
        isLoaded: true,
        temp: split[0] + "°C\n",
        humid: split[1] + "%\n",
        millis: split[2].substring(10),
        duration: "Time worn: " + this.makeDate(split[2].substring(10))
      })
    },
    error => {
      this.setState({
        isLoaded: true,
        error
      })
    });
  }
  makeDate(millis: string){
    let date = ''
    let duration = moment.duration(millis);
    date += duration.days() + "d ";
    date += duration.hours() + "h ";
    date += duration.minutes() + "m ";
    date += duration.seconds() + "s "
    return date;
  }
  componentDidMount() {
    this.getData();
    let timer = setInterval(()=> this.getData(), 5000)
  }
  getHealthColor(){
    let limit = moment.duration(105, 'm').asMilliseconds();
    let percent = parseFloat(this.state.millis) / limit 
    if(percent <= 0.80){
      return '#00ff00'
    }
    else if(percent > .80 && percent <= 0.95){
      return '#ffff00'
    }
    else {
      return '#ff0000'
    }
  }
  getHealthStatus(){
    let limit = moment.duration(105, 'm').asMilliseconds();
    let percent = parseFloat(this.state.millis) / limit 
    if(percent <= 0.80){
      return 'Good'
    }
    else if(percent > .80 && percent <= 0.95){
      return 'OK'
    }
    else {
      return 'Needs Replacing'
    }
  }
  render() {
    return (
      <View style={styles.container}>
        {/* <img src={require('../assets/aid.jpg')}/> */}
        <Text style={styles.title}>Mask-er-Aid</Text>
        <View style={styles.separator} lightColor="#eee" darkColor="rgba(255,255,255,0.1)" />
        <Text>
          {this.state.temp}
          {this.state.humid}
          {this.state.duration}
        </Text>
        <View style={{backgroundColor: this.getHealthColor(), width: '75%', marginTop: 20, height: 100, alignItems: 'center', justifyContent: 'center', borderRadius: 20}}>
        <Text style={{color: '#fff', fontSize: 20}}>
          Mask Health: {this.getHealthStatus()}
        </Text>
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  title: {
    fontSize: 20,
    fontWeight: 'bold',
  },
  separator: {
    marginVertical: 30,
    height: 1,
    width: '80%',
  },
});
